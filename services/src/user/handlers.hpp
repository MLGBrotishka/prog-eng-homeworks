#pragma once

#include <memory>

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

#include "../common/mongo_database.hpp"
#include "../common/models.hpp"
#include "../common/redis_cache.hpp"
#include "../common/rate_limiter.hpp"

namespace taxi_service::user {

// POST /api/v1/users - Create new user
class CreateUserHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-create-user";

    CreateUserHandler(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext&) const override;

private:
    std::shared_ptr<MongoDatabase> db_;
};

// GET /api/v1/users/by-login?login=... - Find user by login (Cache-Aside)
class FindUserByLoginHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-find-user";

    FindUserByLoginHandler(const userver::components::ComponentConfig& config,
                          const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext&) const override;

private:
    std::shared_ptr<MongoDatabase> db_;
    std::shared_ptr<RedisCache> cache_;
};

// GET /api/v1/users/search?mask=... - Search users by name mask
class SearchUsersByNameHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-search-users";

    SearchUsersByNameHandler(const userver::components::ComponentConfig& config,
                            const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext&) const override;

private:
    std::shared_ptr<MongoDatabase> db_;
};

// POST /api/v1/auth/login - Login user (Rate-Limited)
class LoginHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-login";

    LoginHandler(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& context);
    static userver::yaml_config::Schema GetStaticConfigSchema();

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
private:
    std::string GenerateToken(int64_t user_id) const;
    std::shared_ptr<MongoDatabase> db_;
    std::shared_ptr<RateLimiter> rate_limiter_;
    std::string secret_;
    std::string issuer_;
    int token_expiration_hours_;
};

void AppendUserHandlers(userver::components::ComponentList& component_list);

}  // namespace taxi_service::user
