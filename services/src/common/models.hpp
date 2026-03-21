#pragma once

#include <string>
#include <vector>
#include <optional>
#include <chrono>

#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace taxi_service {

struct User {
    std::optional<int64_t> id;
    std::string login;
    std::string email;
    std::string first_name;
    std::string last_name;
    std::string password_hash;
    std::chrono::system_clock::time_point created_at;

    userver::formats::json::Value ToJson() const;
    static User FromJson(const userver::formats::json::Value& json);
};

struct Driver {
    std::optional<int64_t> id;
    int64_t user_id;
    std::string full_name;
    std::string license_plate;
    std::string status;
    std::chrono::system_clock::time_point created_at;

    userver::formats::json::Value ToJson() const;
    static Driver FromJson(const userver::formats::json::Value& json);
};

struct Ride {
    std::optional<int64_t> id;
    int64_t passenger_id;
    std::optional<int64_t> driver_id;
    std::string from_address;
    std::string to_address;
    std::string status;
    std::optional<double> cost;
    std::chrono::system_clock::time_point created_at;

    userver::formats::json::Value ToJson() const;
    static Ride FromJson(const userver::formats::json::Value& json);
};

struct AuthToken {
    std::string token;
    int64_t user_id;
    std::chrono::system_clock::time_point expires_at;

    userver::formats::json::Value ToJson() const;
};

struct LoginRequest {
    std::string login;
    std::string password;

    static LoginRequest FromJson(const userver::formats::json::Value& json);
};

struct CreateUserRequest {
    std::string login;
    std::string email;
    std::string first_name;
    std::string last_name;
    std::string password;

    static CreateUserRequest FromJson(const userver::formats::json::Value& json);
};

struct RegisterDriverRequest {
    std::string full_name;
    std::string license_plate;

    static RegisterDriverRequest FromJson(const userver::formats::json::Value& json);
};

struct CreateRideRequest {
    std::string from_address;
    std::string to_address;

    static CreateRideRequest FromJson(const userver::formats::json::Value& json);
};

}  // namespace taxi_service