#pragma once

#include <memory>

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/auth/auth_checker_base.hpp>

#include "../common/database.hpp"
#include "../common/models.hpp"

namespace taxi_service::driver {

// POST /api/v1/drivers - Register as a driver
class RegisterDriverHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-register-driver";

    RegisterDriverHandler(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;

private:
    std::shared_ptr<Database> db_;
};

void AppendDriverHandlers(userver::components::ComponentList& component_list);

}  // namespace taxi_service::driver
