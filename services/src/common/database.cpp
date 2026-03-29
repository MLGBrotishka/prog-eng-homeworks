#include "database.hpp"

#include <userver/crypto/hash.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <cstdlib>

namespace taxi_service {

namespace {
    std::string HashPassword(const std::string& password) {
        return userver::crypto::hash::Sha256(password);
    }

    bool VerifyPassword(const std::string& password, const std::string& hash) {
        return HashPassword(password) == hash;
    }
}

Database::Database(userver::storages::postgres::ClusterPtr pg_cluster, DatabaseConfig config) 
    : pg_(std::move(pg_cluster)), config_(config) {}

Database::~Database() = default;

std::optional<User> Database::CreateUser(const CreateUserRequest& request) {
    auto hash = HashPassword(request.password);
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO users (login, email, first_name, last_name, password_hash) "
        "VALUES ($1, $2, $3, $4, $5) RETURNING id, login, email, first_name, last_name, password_hash, created_at",
        request.login, request.email, request.first_name, request.last_name, hash);
    if (res.IsEmpty()) return std::nullopt;
    
    auto row = res[0];
    User u;
    u.id = row["id"].As<int64_t>();
    u.login = row["login"].As<std::string>();
    u.email = row["email"].As<std::string>();
    u.first_name = row["first_name"].As<std::string>();
    u.last_name = row["last_name"].As<std::string>();
    u.password_hash = row["password_hash"].As<std::string>();
    u.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return u;
}

std::optional<User> Database::FindUserByLogin(const std::string& login) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE login = $1",
        login);
    if (res.IsEmpty()) return std::nullopt;
    
    auto row = res[0];
    User u;
    u.id = row["id"].As<int64_t>();
    u.login = row["login"].As<std::string>();
    u.email = row["email"].As<std::string>();
    u.first_name = row["first_name"].As<std::string>();
    u.last_name = row["last_name"].As<std::string>();
    u.password_hash = row["password_hash"].As<std::string>();
    u.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return u;
}

std::optional<User> Database::FindUserById(int64_t id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE id = $1",
        id);
    if (res.IsEmpty()) return std::nullopt;
    
    auto row = res[0];
    User u;
    u.id = row["id"].As<int64_t>();
    u.login = row["login"].As<std::string>();
    u.email = row["email"].As<std::string>();
    u.first_name = row["first_name"].As<std::string>();
    u.last_name = row["last_name"].As<std::string>();
    u.password_hash = row["password_hash"].As<std::string>();
    u.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return u;
}

std::vector<User> Database::SearchUsersByNameMask(const std::string& mask) {
    std::string search = "%" + mask + "%";
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE first_name || ' ' || last_name ILIKE $1",
        search);
        
    std::vector<User> results;
    for (auto row : res) {
        User u;
        u.id = row["id"].As<int64_t>();
        u.login = row["login"].As<std::string>();
        u.email = row["email"].As<std::string>();
        u.first_name = row["first_name"].As<std::string>();
        u.last_name = row["last_name"].As<std::string>();
        u.password_hash = row["password_hash"].As<std::string>();
        u.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
        results.push_back(u);
    }
    return results;
}

std::optional<User> Database::AuthenticateUser(const std::string& login, const std::string& password) {
    auto u = FindUserByLogin(login);
    if (!u || !VerifyPassword(password, u->password_hash)) return std::nullopt;
    return u;
}

std::optional<Driver> Database::RegisterDriver(int64_t user_id, const RegisterDriverRequest& request) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO drivers (user_id, full_name, license_plate) "
        "VALUES ($1, $2, $3) RETURNING id, user_id, full_name, license_plate, status, created_at",
        user_id, request.full_name, request.license_plate);
    if (res.IsEmpty()) return std::nullopt;
    
    auto row = res[0];
    Driver d;
    d.id = row["id"].As<int64_t>();
    d.user_id = row["user_id"].As<int64_t>();
    d.full_name = row["full_name"].As<std::string>();
    d.license_plate = row["license_plate"].As<std::string>();
    d.status = row["status"].As<std::string>();
    d.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return d;
}

std::optional<Driver> Database::GetDriverByUserId(int64_t user_id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, user_id, full_name, license_plate, status, created_at FROM drivers WHERE user_id = $1",
        user_id);
    if (res.IsEmpty()) return std::nullopt;
    
    auto row = res[0];
    Driver d;
    d.id = row["id"].As<int64_t>();
    d.user_id = row["user_id"].As<int64_t>();
    d.full_name = row["full_name"].As<std::string>();
    d.license_plate = row["license_plate"].As<std::string>();
    d.status = row["status"].As<std::string>();
    d.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return d;
}

std::optional<Driver> Database::GetDriverById(int64_t driver_id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, user_id, full_name, license_plate, status, created_at FROM drivers WHERE id = $1",
        driver_id);
    if (res.IsEmpty()) return std::nullopt;
    
    auto row = res[0];
    Driver d;
    d.id = row["id"].As<int64_t>();
    d.user_id = row["user_id"].As<int64_t>();
    d.full_name = row["full_name"].As<std::string>();
    d.license_plate = row["license_plate"].As<std::string>();
    d.status = row["status"].As<std::string>();
    d.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return d;
}

std::optional<Ride> Database::CreateRide(int64_t passenger_id, const CreateRideRequest& request) {
    double cost = config_.base_ride_cost + (rand() % static_cast<int>(config_.max_random_ride_cost));
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO rides (passenger_id, from_address, to_address, cost) "
        "VALUES ($1, $2, $3, $4) RETURNING id, passenger_id, driver_id, from_address, to_address, status, cost, created_at",
        passenger_id, request.from_address, request.to_address, cost);
    if (res.IsEmpty()) return std::nullopt;
    
    auto row = res[0];
    Ride r;
    r.id = row["id"].As<int64_t>();
    r.passenger_id = row["passenger_id"].As<int64_t>();
    r.driver_id = row["driver_id"].As<std::optional<int64_t>>();
    r.from_address = row["from_address"].As<std::string>();
    r.to_address = row["to_address"].As<std::string>();
    r.status = row["status"].As<std::string>();
    r.cost = row["cost"].As<std::optional<double>>();
    r.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return r;
}

std::vector<Ride> Database::ListActiveRides() {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, passenger_id, driver_id, from_address, to_address, status, cost, created_at "
        "FROM rides WHERE status != 'completed' ORDER BY created_at DESC");
        
    std::vector<Ride> results;
    for (auto row : res) {
        Ride r;
        r.id = row["id"].As<int64_t>();
        r.passenger_id = row["passenger_id"].As<int64_t>();
        r.driver_id = row["driver_id"].As<std::optional<int64_t>>();
        r.from_address = row["from_address"].As<std::string>();
        r.to_address = row["to_address"].As<std::string>();
        r.status = row["status"].As<std::string>();
        r.cost = row["cost"].As<std::optional<double>>();
        r.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
        results.push_back(r);
    }
    return results;
}

std::optional<Ride> Database::GetRideById(int64_t ride_id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, passenger_id, driver_id, from_address, to_address, status, cost, created_at "
        "FROM rides WHERE id = $1",
        ride_id);
    if (res.IsEmpty()) return std::nullopt;
    
    auto row = res[0];
    Ride r;
    r.id = row["id"].As<int64_t>();
    r.passenger_id = row["passenger_id"].As<int64_t>();
    r.driver_id = row["driver_id"].As<std::optional<int64_t>>();
    r.from_address = row["from_address"].As<std::string>();
    r.to_address = row["to_address"].As<std::string>();
    r.status = row["status"].As<std::string>();
    r.cost = row["cost"].As<std::optional<double>>();
    r.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return r;
}

std::vector<Ride> Database::GetUserRideHistory(int64_t user_id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, passenger_id, driver_id, from_address, to_address, status, cost, created_at "
        "FROM rides WHERE passenger_id = $1 ORDER BY created_at DESC",
        user_id);
        
    std::vector<Ride> results;
    for (auto row : res) {
        Ride r;
        r.id = row["id"].As<int64_t>();
        r.passenger_id = row["passenger_id"].As<int64_t>();
        r.driver_id = row["driver_id"].As<std::optional<int64_t>>();
        r.from_address = row["from_address"].As<std::string>();
        r.to_address = row["to_address"].As<std::string>();
        r.status = row["status"].As<std::string>();
        r.cost = row["cost"].As<std::optional<double>>();
        r.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
        results.push_back(r);
    }
    return results;
}

bool Database::AcceptRide(int64_t ride_id, int64_t driver_id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE rides SET driver_id = $1, status = 'accepted' WHERE id = $2 AND status = 'pending'",
        driver_id, ride_id);
    return res.RowsAffected() > 0;
}

bool Database::CompleteRide(int64_t ride_id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE rides SET status = 'completed' WHERE id = $1 AND status = 'accepted'",
        ride_id);
    return res.RowsAffected() > 0;
}

}  // namespace taxi_service
