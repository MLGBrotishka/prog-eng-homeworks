#include "models.hpp"

#include <userver/formats/json/serialize.hpp>
#include <userver/utils/datetime.hpp>

namespace taxi_service {

// User
userver::formats::json::Value User::ToJson() const {
    userver::formats::json::ValueBuilder builder;
    if (id) {
        builder["id"] = *id;
    }
    builder["login"] = login;
    builder["email"] = email;
    builder["first_name"] = first_name;
    builder["last_name"] = last_name;
    builder["created_at"] = userver::utils::datetime::Timestring(created_at);
    return builder.ExtractValue();
}

User User::FromJson(const userver::formats::json::Value& json) {
    User user;
    if (json.HasMember("id")) {
        user.id = json["id"].As<int64_t>();
    }
    user.login = json["login"].As<std::string>();
    user.email = json["email"].As<std::string>();
    user.first_name = json["first_name"].As<std::string>();
    user.last_name = json["last_name"].As<std::string>();
    if (json.HasMember("password")) {
        user.password_hash = json["password"].As<std::string>();
    }
    return user;
}

// Driver
userver::formats::json::Value Driver::ToJson() const {
    userver::formats::json::ValueBuilder builder;
    if (id) {
        builder["id"] = *id;
    }
    builder["user_id"] = user_id;
    builder["full_name"] = full_name;
    builder["license_plate"] = license_plate;
    builder["status"] = status;
    builder["created_at"] = userver::utils::datetime::Timestring(created_at);
    return builder.ExtractValue();
}

Driver Driver::FromJson(const userver::formats::json::Value& json) {
    Driver driver;
    if (json.HasMember("id")) {
        driver.id = json["id"].As<int64_t>();
    }
    if (json.HasMember("user_id")) {
        driver.user_id = json["user_id"].As<int64_t>();
    }
    driver.full_name = json["full_name"].As<std::string>();
    driver.license_plate = json["license_plate"].As<std::string>();
    if (json.HasMember("status")) {
        driver.status = json["status"].As<std::string>();
    }
    return driver;
}

// Ride
userver::formats::json::Value Ride::ToJson() const {
    userver::formats::json::ValueBuilder builder;
    if (id) {
        builder["id"] = *id;
    }
    builder["passenger_id"] = passenger_id;
    if (driver_id) {
        builder["driver_id"] = *driver_id;
    }
    builder["from_address"] = from_address;
    builder["to_address"] = to_address;
    builder["status"] = status;
    if (cost) {
        builder["cost"] = *cost;
    }
    builder["created_at"] = userver::utils::datetime::Timestring(created_at);
    return builder.ExtractValue();
}

Ride Ride::FromJson(const userver::formats::json::Value& json) {
    Ride ride;
    if (json.HasMember("id")) {
        ride.id = json["id"].As<int64_t>();
    }
    if (json.HasMember("passenger_id")) {
        ride.passenger_id = json["passenger_id"].As<int64_t>();
    }
    if (json.HasMember("driver_id")) {
        ride.driver_id = json["driver_id"].As<int64_t>();
    }
    ride.from_address = json["from_address"].As<std::string>();
    ride.to_address = json["to_address"].As<std::string>();
    if (json.HasMember("status")) {
        ride.status = json["status"].As<std::string>();
    }
    if (json.HasMember("cost")) {
        ride.cost = json["cost"].As<double>();
    }
    return ride;
}

// AuthToken
userver::formats::json::Value AuthToken::ToJson() const {
    userver::formats::json::ValueBuilder builder;
    builder["token"] = token;
    builder["user_id"] = user_id;
    builder["expires_at"] = userver::utils::datetime::Timestring(expires_at);
    return builder.ExtractValue();
}

// LoginRequest
LoginRequest LoginRequest::FromJson(const userver::formats::json::Value& json) {
    LoginRequest request;
    request.login = json["login"].As<std::string>();
    request.password = json["password"].As<std::string>();
    return request;
}

// CreateUserRequest
CreateUserRequest CreateUserRequest::FromJson(const userver::formats::json::Value& json) {
    CreateUserRequest request;
    request.login = json["login"].As<std::string>();
    request.email = json["email"].As<std::string>();
    request.first_name = json["first_name"].As<std::string>();
    request.last_name = json["last_name"].As<std::string>();
    request.password = json["password"].As<std::string>();
    return request;
}

// RegisterDriverRequest
RegisterDriverRequest RegisterDriverRequest::FromJson(const userver::formats::json::Value& json) {
    RegisterDriverRequest request;
    request.full_name = json["full_name"].As<std::string>();
    request.license_plate = json["license_plate"].As<std::string>();
    return request;
}

// CreateRideRequest
CreateRideRequest CreateRideRequest::FromJson(const userver::formats::json::Value& json) {
    CreateRideRequest request;
    request.from_address = json["from_address"].As<std::string>();
    request.to_address = json["to_address"].As<std::string>();
    return request;
}

}  // namespace taxi_service