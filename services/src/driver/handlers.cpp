#include "handlers.hpp"

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

namespace taxi_service::driver {

int64_t ExtractUserIdFromContext(const userver::server::request::RequestContext& context) {
    return context.GetData<int64_t>("user_id");
}

RegisterDriverHandler::RegisterDriverHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    db_ = std::make_shared<Database>("/app/data/taxi.db");
}

std::string RegisterDriverHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    auto user_id = ExtractUserIdFromContext(context);
    
    auto request_body = userver::formats::json::FromString(request.RequestBody());
    
    if (!request_body.HasMember("full_name") || !request_body.HasMember("license_plate")) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Full name and license plate are required")
        );
    }
    
    auto register_request = RegisterDriverRequest::FromJson(request_body);

    auto driver = db_->RegisterDriver(user_id, register_request);
    if (!driver) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Already registered as driver or invalid data")
        );
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ToString(driver->ToJson());
}

void AppendDriverHandlers(userver::components::ComponentList& component_list) {
    component_list.Append<RegisterDriverHandler>();
}

}  // namespace taxi_service::driver
