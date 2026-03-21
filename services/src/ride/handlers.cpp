#include "handlers.hpp"

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

namespace taxi_service::ride {

int64_t ExtractUserIdFromContext(const userver::server::request::RequestContext& context) {
    return context.GetData<int64_t>("user_id");
}

CreateRideHandler::CreateRideHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    db_ = std::make_shared<Database>("/app/data/taxi.db");
}

std::string CreateRideHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    auto passenger_id = ExtractUserIdFromContext(context);
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
        return userver::formats::json::ToString(userver::formats::json::MakeObject("error", "Failed to create ride"));
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ToString(ride->ToJson());
}

ListActiveRidesHandler::ListActiveRidesHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    db_ = std::make_shared<Database>("/app/data/taxi.db");
}

std::string ListActiveRidesHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    auto status = request.GetArg("status");
    if (status != "active") {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(userver::formats::json::MakeObject("error", "Invalid status, only 'active' supported here"));
    }
    
    auto rides = db_->ListActiveRides();
    userver::formats::json::ValueBuilder builder(userver::formats::json::Type::kArray);
    for (const auto& r : rides) builder.PushBack(r.ToJson());
    return userver::formats::json::ToString(builder.ExtractValue());
}

AcceptRideHandler::AcceptRideHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    db_ = std::make_shared<Database>("/app/data/taxi.db");
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
        return userver::formats::json::ToString(userver::formats::json::MakeObject("error", "Only drivers can accept rides"));
    }
    
    if (!db_->AcceptRide(ride_id, *driver->id)) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(userver::formats::json::MakeObject("error", "Could not accept ride"));
    }
    
    return "{}";
}

CompleteRideHandler::CompleteRideHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    db_ = std::make_shared<Database>("/app/data/taxi.db");
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
        return userver::formats::json::ToString(userver::formats::json::MakeObject("error", "Only drivers can complete rides"));
    }
    
    auto ride = db_->GetRideById(ride_id);
    if (!ride || ride->driver_id != driver->id) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kForbidden);
        return userver::formats::json::ToString(userver::formats::json::MakeObject("error", "You are not the driver for this ride"));
    }
    
    if (!db_->CompleteRide(ride_id)) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(userver::formats::json::MakeObject("error", "Could not complete ride"));
    }
    
    return "{}";
}

UserRideHistoryHandler::UserRideHistoryHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    db_ = std::make_shared<Database>("/app/data/taxi.db");
}

std::string UserRideHistoryHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    auto auth_user_id = ExtractUserIdFromContext(context);
    auto target_user_id_str = request.GetPathArg("id");
    int64_t target_user_id = std::stoll(target_user_id_str);
    
    if (auth_user_id != target_user_id) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kForbidden);
        return userver::formats::json::ToString(userver::formats::json::MakeObject("error", "Access denied to other user's history"));
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
