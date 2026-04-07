#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <userver/storages/postgres/cluster.hpp>

#include "models.hpp"

namespace taxi_service {

struct DatabaseConfig {
    double base_ride_cost = 10.0;
    double max_random_ride_cost = 40.0;
};

class Database {
public:
    explicit Database(userver::storages::postgres::ClusterPtr pg_cluster, 
                     DatabaseConfig config = {});
    ~Database();

    // User operations
    std::optional<User> CreateUser(const CreateUserRequest& request);
    std::optional<User> FindUserByLogin(const std::string& login);
    std::optional<User> FindUserById(int64_t id);
    std::vector<User> SearchUsersByNameMask(const std::string& mask);
    std::optional<User> AuthenticateUser(const std::string& login, const std::string& password);

    // Driver operations
    std::optional<Driver> RegisterDriver(int64_t user_id, const RegisterDriverRequest& request);
    std::optional<Driver> GetDriverByUserId(int64_t user_id);
    std::optional<Driver> GetDriverById(int64_t driver_id);

    // Ride operations
    std::optional<Ride> CreateRide(int64_t passenger_id, const CreateRideRequest& request);
    std::vector<Ride> ListActiveRides();
    std::optional<Ride> GetRideById(int64_t ride_id);
    std::vector<Ride> GetUserRideHistory(int64_t user_id);
    bool AcceptRide(int64_t ride_id, int64_t driver_id);
    bool CompleteRide(int64_t ride_id);

private:
    userver::storages::postgres::ClusterPtr pg_;
    DatabaseConfig config_;
};

}  // namespace taxi_service
