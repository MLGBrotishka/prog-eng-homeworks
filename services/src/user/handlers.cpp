#include "handlers.hpp"

#include <userver/components/component.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/storages/mongo/exception.hpp>
#include <userver/storages/redis/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/utils/datetime.hpp>
#include <jwt-cpp/jwt.h>
#include <userver/yaml_config/merge_schemas.hpp>

namespace taxi_service::user {

namespace {

/// Extract best available client IP from request headers.
std::string GetClientIp(const userver::server::http::HttpRequest& request) {
    auto ip = request.GetHeader("X-Real-IP");
    if (!ip.empty()) return ip;
    ip = request.GetHeader("X-Forwarded-For");
    if (!ip.empty()) {
        // X-Forwarded-For may be a comma-separated list; take the first entry.
        const auto comma = ip.find(',');
        return (comma != std::string::npos) ? ip.substr(0, comma) : ip;
    }
    return "unknown";
}

/// Apply rate-limit headers and return 429 if limit exceeded.
/// Returns true if the request should be rejected.
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

}  // namespace

// ---------------------------------------------------------------------------
// CreateUserHandler
// ---------------------------------------------------------------------------

CreateUserHandler::CreateUserHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& mongo_component = context.FindComponent<userver::components::Mongo>("mongo-taxi-db");
    db_ = std::make_shared<MongoDatabase>(mongo_component.GetPool());
}

std::string CreateUserHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    auto request_body = userver::formats::json::FromString(request.RequestBody());

    if (!request_body.HasMember("login") || !request_body.HasMember("email") ||
        !request_body.HasMember("first_name") || !request_body.HasMember("last_name") ||
        !request_body.HasMember("password")) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Missing required fields")
        );
    }

    auto create_request = CreateUserRequest::FromJson(request_body);

    if (create_request.login.empty() || create_request.email.empty() ||
        create_request.first_name.empty() || create_request.last_name.empty() ||
        create_request.password.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Missing required fields")
        );
    }

    if (create_request.email.find('@') == std::string::npos) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Invalid email format")
        );
    }

    try {
        auto user = db_->CreateUser(create_request);
        if (!user) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
            return userver::formats::json::ToString(
                userver::formats::json::MakeObject("error", "User already exists")
            );
        }
        request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
        return userver::formats::json::ToString(user->ToJson());
    } catch (const std::exception& ex) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kInternalServerError);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Internal server error: " + std::string(ex.what()))
        );
    }
}

// ---------------------------------------------------------------------------
// FindUserByLoginHandler — Cache-Aside, TTL=5min
// ---------------------------------------------------------------------------

FindUserByLoginHandler::FindUserByLoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& mongo_component = context.FindComponent<userver::components::Mongo>("mongo-taxi-db");
    db_ = std::make_shared<MongoDatabase>(mongo_component.GetPool());
    cache_ = std::make_shared<RedisCache>(GetRedisClient(context));
}

std::string FindUserByLoginHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    auto login = request.GetArg("login");
    if (login.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Missing 'login' parameter")
        );
    }

    // Cache-Aside: try cache first
    const std::string cache_key = "user:login:" + login;
    auto cached = cache_->Get(cache_key);
    if (cached) {
        return *cached;
    }

    // Cache miss: query MongoDB
    auto user = db_->FindUserByLogin(login);
    if (!user) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "User not found")
        );
    }

    const auto json_str = userver::formats::json::ToString(user->ToJson());
    // Cache for 5 minutes (profiles change rarely)
    cache_->Set(cache_key, json_str, std::chrono::seconds{300});
    return json_str;
}

// ---------------------------------------------------------------------------
// SearchUsersByNameHandler
// ---------------------------------------------------------------------------

SearchUsersByNameHandler::SearchUsersByNameHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& mongo_component = context.FindComponent<userver::components::Mongo>("mongo-taxi-db");
    db_ = std::make_shared<MongoDatabase>(mongo_component.GetPool());
}

std::string SearchUsersByNameHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    auto mask = request.GetArg("mask");
    if (mask.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Missing 'mask' parameter")
        );
    }

    auto users = db_->SearchUsersByNameMask(mask);

    userver::formats::json::ValueBuilder builder(userver::formats::json::Type::kArray);
    for (const auto& user : users) {
        builder.PushBack(user.ToJson());
    }

    return userver::formats::json::ToString(builder.ExtractValue());
}

// ---------------------------------------------------------------------------
// LoginHandler — Rate limited: 5 req/60s per IP
// ---------------------------------------------------------------------------

LoginHandler::LoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& mongo_component = context.FindComponent<userver::components::Mongo>("mongo-taxi-db");
    db_ = std::make_shared<MongoDatabase>(mongo_component.GetPool());
    rate_limiter_ = std::make_shared<RateLimiter>(GetRedisClient(context));
    secret_ = config["jwt-secret"].As<std::string>();
    issuer_ = config["jwt-issuer"].As<std::string>("taxi-service");
    token_expiration_hours_ = config["token-expiration-hours"].As<int>(24);
}

userver::yaml_config::Schema LoginHandler::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<HttpHandlerBase>(R"(
type: object
description: login handler config
additionalProperties: false
properties:
    jwt-secret:
        type: string
        description: secret key for JWT signing
    jwt-issuer:
        type: string
        description: issuer for JWT signing
    token-expiration-hours:
        type: integer
        description: expiration time for JWT in hours
)");
}

std::string LoginHandler::GenerateToken(int64_t user_id) const {
    auto token = ::jwt::create<::jwt::traits::kazuho_picojson>()
        .set_issuer(issuer_)
        .set_type("JWT")
        .set_payload_claim("user_id", ::jwt::claim(std::to_string(user_id)))
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(token_expiration_hours_))
        .sign(::jwt::algorithm::hs256{secret_});
    return token;
}

std::string LoginHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& /*context*/) const {

    // Rate limiting: 5 req/60s per client IP
    const std::string ip = GetClientIp(request);
    if (ApplyRateLimit(request, *rate_limiter_,
                       "login:" + ip, 5, std::chrono::seconds{60})) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kTooManyRequests);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Too Many Requests")
        );
    }

    auto request_body = userver::formats::json::FromString(request.RequestBody());

    if (!request_body.HasMember("login")) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Login is required")
        );
    }
    if (!request_body.HasMember("password")) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Password is required")
        );
    }

    auto login_request = LoginRequest::FromJson(request_body);

    if (login_request.login.empty() || login_request.password.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Missing required fields")
        );
    }

    auto user = db_->AuthenticateUser(login_request.login, login_request.password);
    if (!user) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Invalid credentials")
        );
    }

    AuthToken token;
    token.token = GenerateToken(*user->id);
    token.user_id = *user->id;
    token.expires_at = std::chrono::system_clock::now() + std::chrono::hours(token_expiration_hours_);

    return userver::formats::json::ToString(token.ToJson());
}

void AppendUserHandlers(userver::components::ComponentList& component_list) {
    component_list.Append<CreateUserHandler>();
    component_list.Append<FindUserByLoginHandler>();
    component_list.Append<SearchUsersByNameHandler>();
    component_list.Append<LoginHandler>();
}

}  // namespace taxi_service::user
