#pragma once

#include <memory>

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/auth/auth_checker_base.hpp>

#include "../common/database.hpp"
#include "../common/models.hpp"
#include "../common/redis_cache.hpp"
#include "../common/rate_limiter.hpp"

namespace taxi_service::ride {

// POST /api/v1/rides - Create ride request (Rate-Limited)
class CreateRideHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-create-ride";

    CreateRideHandler(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& context);
    static userver::yaml_config::Schema GetStaticConfigSchema();

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;

private:
    std::shared_ptr<Database> db_;
    std::shared_ptr<RateLimiter> rate_limiter_;
};

// GET /api/v1/rides?status=active - List active rides (Cache-Aside + Rate-Limited)
class ListActiveRidesHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-list-active-rides";

    ListActiveRidesHandler(const userver::components::ComponentConfig& config,
                          const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;

private:
    std::shared_ptr<Database> db_;
    std::shared_ptr<RedisCache> cache_;
    std::shared_ptr<RateLimiter> rate_limiter_;
};

// PATCH /api/v1/rides/{id}/accept - Accept ride (invalidates cache)
class AcceptRideHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-accept-ride";

    AcceptRideHandler(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;

private:
    std::shared_ptr<Database> db_;
    std::shared_ptr<RedisCache> cache_;
};

// PATCH /api/v1/rides/{id}/complete - Complete ride (invalidates cache)
class CompleteRideHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-complete-ride";

    CompleteRideHandler(const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;

private:
    std::shared_ptr<Database> db_;
    std::shared_ptr<RedisCache> cache_;
};

// GET /api/v1/users/{id}/rides - Ride history
class UserRideHistoryHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-user-ride-history";

    UserRideHistoryHandler(const userver::components::ComponentConfig& config,
                          const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;

private:
    std::shared_ptr<Database> db_;
};

void AppendRideHandlers(userver::components::ComponentList& component_list);

}  // namespace taxi_service::ride
