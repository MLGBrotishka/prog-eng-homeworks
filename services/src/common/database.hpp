#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <sqlite3.h>

#include "models.hpp"

namespace taxi_service {

class Database {
public:
    explicit Database(const std::string& db_path = "taxi.db");
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

    // Initialize database schema
    void InitSchema();

private:
    sqlite3* db_;
    
    std::string HashPassword(const std::string& password) const;
    bool VerifyPassword(const std::string& password, const std::string& hash) const;
    
    // Helper methods for SQLite operations
    bool ExecuteSQL(const std::string& sql);
    std::optional<int64_t> GetLastInsertId();
};

}  // namespace taxi_service
