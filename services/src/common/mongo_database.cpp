#include "mongo_database.hpp"

#include <userver/crypto/hash.hpp>
#include <userver/formats/bson.hpp>
#include <userver/storages/mongo/exception.hpp>
#include <userver/utils/datetime.hpp>
#include <chrono>

namespace taxi_service {

namespace {
    std::string HashPassword(const std::string& password) {
        return userver::crypto::hash::Sha256(password);
    }

    bool VerifyPassword(const std::string& password, const std::string& hash) {
        return HashPassword(password) == hash;
    }
}

MongoDatabase::MongoDatabase(userver::storages::mongo::PoolPtr mongo_pool) 
    : mongo_(std::move(mongo_pool)) {}

MongoDatabase::~MongoDatabase() = default;

std::optional<User> MongoDatabase::CreateUser(const CreateUserRequest& request) {
    auto hash = HashPassword(request.password);
    int64_t new_id = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
        
    auto created_at = std::chrono::system_clock::now();

    auto doc = userver::formats::bson::MakeDoc(
        "_id", new_id,
        "login", request.login,
        "email", request.email,
        "first_name", request.first_name,
        "last_name", request.last_name,
        "password_hash", hash,
        "created_at", created_at
    );

    auto users = mongo_->GetCollection("users");
    
    try {
        users.InsertOne(doc);
    } catch (const userver::storages::mongo::DuplicateKeyException&) {
        return std::nullopt;
    }

    User u;
    u.id = new_id;
    u.login = request.login;
    u.email = request.email;
    u.first_name = request.first_name;
    u.last_name = request.last_name;
    u.password_hash = hash;
    u.created_at = created_at;
    
    return u;
}

std::optional<User> MongoDatabase::FindUserByLogin(const std::string& login) {
    auto users = mongo_->GetCollection("users");
    auto res = users.FindOne(userver::formats::bson::MakeDoc("login", login));
    if (!res) return std::nullopt;
    
    User u;
    u.id = (*res)["_id"].As<int64_t>();
    u.login = (*res)["login"].As<std::string>();
    u.email = (*res)["email"].As<std::string>();
    u.first_name = (*res)["first_name"].As<std::string>();
    u.last_name = (*res)["last_name"].As<std::string>();
    u.password_hash = (*res)["password_hash"].As<std::string>();
    u.created_at = (*res)["created_at"].As<std::chrono::system_clock::time_point>();
    return u;
}

std::optional<User> MongoDatabase::FindUserById(int64_t id) {
    auto users = mongo_->GetCollection("users");
    auto res = users.FindOne(userver::formats::bson::MakeDoc("_id", id));
    if (!res) return std::nullopt;
    
    User u;
    u.id = (*res)["_id"].As<int64_t>();
    u.login = (*res)["login"].As<std::string>();
    u.email = (*res)["email"].As<std::string>();
    u.first_name = (*res)["first_name"].As<std::string>();
    u.last_name = (*res)["last_name"].As<std::string>();
    u.password_hash = (*res)["password_hash"].As<std::string>();
    u.created_at = (*res)["created_at"].As<std::chrono::system_clock::time_point>();
    return u;
}

std::vector<User> MongoDatabase::SearchUsersByNameMask(const std::string& mask) {
    auto users = mongo_->GetCollection("users");
    
    userver::formats::bson::ValueBuilder builder;
    builder["$or"] = userver::formats::bson::MakeArray(
        userver::formats::bson::MakeDoc("first_name", userver::formats::bson::MakeDoc("$regex", mask, "$options", "i")),
        userver::formats::bson::MakeDoc("last_name", userver::formats::bson::MakeDoc("$regex", mask, "$options", "i"))
    );
    
    auto cursor = users.Find(builder.ExtractValue());
    std::vector<User> results;
    for (const auto& doc : cursor) {
        User u;
        u.id = doc["_id"].As<int64_t>();
        u.login = doc["login"].As<std::string>();
        u.email = doc["email"].As<std::string>();
        u.first_name = doc["first_name"].As<std::string>();
        u.last_name = doc["last_name"].As<std::string>();
        u.password_hash = doc["password_hash"].As<std::string>();
        u.created_at = doc["created_at"].As<std::chrono::system_clock::time_point>();
        results.push_back(u);
    }
    return results;
}

std::optional<User> MongoDatabase::AuthenticateUser(const std::string& login, const std::string& password) {
    auto u = FindUserByLogin(login);
    if (!u || !VerifyPassword(password, u->password_hash)) return std::nullopt;
    return u;
}

}  // namespace taxi_service