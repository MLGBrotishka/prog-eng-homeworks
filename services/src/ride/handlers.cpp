#include "handlers.hpp"

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/redis/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <fmt/format.h>

namespace taxi_service::ride {

namespace {

int64_t ExtractUserIdFromContext(const userver::server::request::RequestContext& context) {
    return context.GetData<int64_t>("user_id");
}

/// Extract best available client IP from request headers.
std::string GetClientIp(const userver::server::http::HttpRequest& request) {
    auto ip = request.GetHeader("X-Real-IP");
    if (!ip.empty()) return ip;
    ip = request.GetHeader("X-Forwarded-For");
    if (!ip.empty()) {
        const auto comma = ip.find(',');
        return (comma != std::string::npos) ? ip.substr(0, comma) : ip;
    }
    return "unknown";
}

/// Apply rate-limit headers and return true if request should be rejected.
bool ApplyRateLimit(const userver::server::http::HttpRequest& request,
                    RateLimiter& limiter,
                    const std::string& key,
                    int64_t limit,
                    std::chrono::seconds window) {
    auto rl = limiter.Check(key, limit, window);
    request.GetHttpResponse().SetHeader(
        std::string{"X-RateLimit-Limit"}, std::to_string(rl.limit));
    request.GetHttpResponse().SetHeader(
        std::string{"X-RateLimit-Remaining"}, std::to_string(rl.remaining));
    request.GetHttpResponse().SetHeader(
        std::string{"X-RateLimit-Reset"}, std::to_string(rl.reset_ts));
    return !rl.allowed;
}

userver::storages::redis::ClientPtr GetRedisClient(
    const userver::components::ComponentContext& context) {
    return context.FindComponent<userver::components::Redis>("redis-taxi")
        .GetClient("taxi-db");
}

static constexpr std::string_view kActiveRidesCacheKey = "rides:active";

}  // namespace

// ---------------------------------------------------------------------------
// CreateRideHandler — Rate limited: 10 req/60s per passenger_id
// ---------------------------------------------------------------------------

CreateRideHandler::CreateRideHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& pg_component = context.FindComponent<userver::components::Postgres>("postgres-taxi-db");
    DatabaseConfig db_config;
    db_config.base_ride_cost = config["base-ride-cost"].As<double>(10.0);
    db_config.max_random_ride_cost = config["max-random-ride-cost"].As<double>(40.0);
    db_ = std::make_shared<Database>(pg_component.GetCluster(), db_config);
    rate_limiter_ = std::make_shared<RateLimiter>(GetRedisClient(context));
}

userver::yaml_config::Schema CreateRideHandler::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<HttpHandlerBase>(R"(
type: object
description: create ride handler config
additionalProperties: false
properties:
    base-ride-cost:
        type: number
        description: base cost for a ride
    max-random-ride-cost:
        type: number
        description: max random additional cost for a ride
)");
}

std::string CreateRideHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {

    auto passenger_id = ExtractUserIdFromContext(context);

    // Rate limiting: 10 req/60s per passenger
    if (ApplyRateLimit(request, *rate_limiter_,
                       "create-ride:" + std::to_string(passenger_id),
                       10, std::chrono::seconds{60})) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kTooManyRequests);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Too Many Requests")
        );
    }

    auto request_body = userver::formats::json::FromString(request.RequestBody());

    if (!request_body.HasMember("from_address") || !request_body.HasMember("to_address")) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Addresses are required")
        );
    }

    auto create_request = CreateRideRequest::FromJson(request_body);
    auto ride = db_->CreateRide(passenger_id, create_request);

    if (!ride) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kInternalServerError);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Failed to create ride"));
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ToString(ride->ToJson());
}

// ---------------------------------------------------------------------------
// ListActiveRidesHandler — Cache-Aside (TTL=10s) + Rate limited: 30/60s per IP
// ---------------------------------------------------------------------------

ListActiveRidesHandler::ListActiveRidesHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& pg_component = context.FindComponent<userver::components::Postgres>("postgres-taxi-db");
    db_ = std::make_shared<Database>(pg_component.GetCluster());
    auto redis_client = GetRedisClient(context);
    cache_ = std::make_shared<RedisCache>(redis_client);
    rate_limiter_ = std::make_shared<RateLimiter>(redis_client);
}

std::string ListActiveRidesHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    auto status = request.GetArg("status");
    if (status != "active") {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Invalid status, only 'active' supported here"));
    }

    // Rate limiting: 30 req/60s per IP
    const std::string ip = GetClientIp(request);
    if (ApplyRateLimit(request, *rate_limiter_,
                       "list-rides:" + ip, 30, std::chrono::seconds{60})) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kTooManyRequests);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Too Many Requests")
        );
    }

    // Cache-Aside: check Redis first
    auto cached = cache_->Get(std::string{kActiveRidesCacheKey});
    if (cached) {
        return *cached;
    }

    // Cache miss: query PostgreSQL
    auto rides = db_->ListActiveRides();
    userver::formats::json::ValueBuilder builder(userver::formats::json::Type::kArray);
    for (const auto& r : rides) builder.PushBack(r.ToJson());
    const auto json_str = userver::formats::json::ToString(builder.ExtractValue());

    // Store in cache for 10 seconds
    cache_->Set(std::string{kActiveRidesCacheKey}, json_str, std::chrono::seconds{10});
    return json_str;
}

// ---------------------------------------------------------------------------
// AcceptRideHandler — invalidates active rides cache on success
// ---------------------------------------------------------------------------

AcceptRideHandler::AcceptRideHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& pg_component = context.FindComponent<userver::components::Postgres>("postgres-taxi-db");
    db_ = std::make_shared<Database>(pg_component.GetCluster());
    cache_ = std::make_shared<RedisCache>(GetRedisClient(context));
}

std::string AcceptRideHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {

    auto user_id = ExtractUserIdFromContext(context);
    auto ride_id_str = request.GetPathArg("id");
    int64_t ride_id = std::stoll(ride_id_str);

    auto driver = db_->GetDriverByUserId(user_id);
    if (!driver) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kForbidden);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Only drivers can accept rides"));
    }

    if (!db_->AcceptRide(ride_id, *driver->id)) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Could not accept ride"));
    }

    // Invalidate cached active rides list — state has changed
    cache_->Del(std::string{kActiveRidesCacheKey});

    return "{}";
}

// ---------------------------------------------------------------------------
// CompleteRideHandler — invalidates active rides cache on success
// ---------------------------------------------------------------------------

CompleteRideHandler::CompleteRideHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& pg_component = context.FindComponent<userver::components::Postgres>("postgres-taxi-db");
    db_ = std::make_shared<Database>(pg_component.GetCluster());
    cache_ = std::make_shared<RedisCache>(GetRedisClient(context));
}

std::string CompleteRideHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {

    auto user_id = ExtractUserIdFromContext(context);
    auto ride_id_str = request.GetPathArg("id");
    int64_t ride_id = std::stoll(ride_id_str);

    auto driver = db_->GetDriverByUserId(user_id);
    if (!driver) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kForbidden);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Only drivers can complete rides"));
    }

    auto ride = db_->GetRideById(ride_id);
    if (!ride || ride->driver_id != driver->id) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kForbidden);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "You are not the driver for this ride"));
    }

    if (!db_->CompleteRide(ride_id)) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Could not complete ride"));
    }

    // Invalidate cached active rides list — state has changed
    cache_->Del(std::string{kActiveRidesCacheKey});

    return "{}";
}

// ---------------------------------------------------------------------------
// UserRideHistoryHandler
// ---------------------------------------------------------------------------

UserRideHistoryHandler::UserRideHistoryHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& pg_component = context.FindComponent<userver::components::Postgres>("postgres-taxi-db");
    db_ = std::make_shared<Database>(pg_component.GetCluster());
}

std::string UserRideHistoryHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {

    auto auth_user_id = ExtractUserIdFromContext(context);
    auto target_user_id_str = request.GetPathArg("id");
    int64_t target_user_id = std::stoll(target_user_id_str);

    if (auth_user_id != target_user_id) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kForbidden);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Access denied to other user's history"));
    }

    auto rides = db_->GetUserRideHistory(target_user_id);
    userver::formats::json::ValueBuilder builder(userver::formats::json::Type::kArray);
    for (const auto& r : rides) builder.PushBack(r.ToJson());
    return userver::formats::json::ToString(builder.ExtractValue());
}

void AppendRideHandlers(userver::components::ComponentList& component_list) {
    component_list.Append<CreateRideHandler>();
    component_list.Append<ListActiveRidesHandler>();
    component_list.Append<AcceptRideHandler>();
    component_list.Append<CompleteRideHandler>();
    component_list.Append<UserRideHistoryHandler>();
}

}  // namespace taxi_service::ride
