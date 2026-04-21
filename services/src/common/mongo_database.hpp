#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <userver/storages/mongo/pool.hpp>

#include "models.hpp"

namespace taxi_service {

class MongoDatabase {
public:
    explicit MongoDatabase(userver::storages::mongo::PoolPtr mongo_pool);
    ~MongoDatabase();

    // User operations
    std::optional<User> CreateUser(const CreateUserRequest& request);
    std::optional<User> FindUserByLogin(const std::string& login);
    std::optional<User> FindUserById(int64_t id);
    std::vector<User> SearchUsersByNameMask(const std::string& mask);
    std::optional<User> AuthenticateUser(const std::string& login, const std::string& password);

private:
    userver::storages::mongo::PoolPtr mongo_;
};

}  // namespace taxi_service