#include "database.hpp"

#include <userver/crypto/hash.hpp>
#include <sstream>
#include <ctime>
#include <iostream>
#include <iomanip>

namespace taxi_service {

std::chrono::system_clock::time_point ParseDateTime(const std::string& datetime_str) {
    if (datetime_str.empty()) {
        return std::chrono::system_clock::now();
    }
    
    std::tm tm = {};
    std::istringstream ss(datetime_str);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    if (ss.fail()) {
        return std::chrono::system_clock::now();
    }
    
    std::time_t time_t_value = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time_t_value);
}

Database::Database(const std::string& db_path) : db_(nullptr) {
    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Cannot open database: " + std::string(sqlite3_errmsg(db_)));
    }
    
    char* err_msg = nullptr;
    rc = sqlite3_exec(db_, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::string error = err_msg ? err_msg : "Unknown error";
        sqlite3_free(err_msg);
        std::cerr << "Failed to enable WAL mode: " << error << std::endl;
    }
    
    sqlite3_busy_timeout(db_, 5000);
    
    InitSchema();
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
    }
}

void Database::InitSchema() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            login TEXT UNIQUE NOT NULL,
            email TEXT UNIQUE NOT NULL,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            password_hash TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );

        CREATE TABLE IF NOT EXISTS drivers (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER UNIQUE NOT NULL,
            full_name TEXT NOT NULL,
            license_plate TEXT NOT NULL,
            status TEXT DEFAULT 'available',
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS rides (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            passenger_id INTEGER NOT NULL,
            driver_id INTEGER,
            from_address TEXT NOT NULL,
            to_address TEXT NOT NULL,
            status TEXT DEFAULT 'pending',
            cost REAL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (passenger_id) REFERENCES users(id),
            FOREIGN KEY (driver_id) REFERENCES drivers(id)
        );

        CREATE INDEX IF NOT EXISTS idx_users_login ON users(login);
        CREATE INDEX IF NOT EXISTS idx_drivers_user_id ON drivers(user_id);
        CREATE INDEX IF NOT EXISTS idx_rides_passenger_id ON rides(passenger_id);
    )";
    
    ExecuteSQL(sql);
}

bool Database::ExecuteSQL(const std::string& sql) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::string error = err_msg ? err_msg : "Unknown error";
        sqlite3_free(err_msg);
        std::cerr << "SQL Error: " << error << std::endl;
        return false;
    }
    return true;
}

std::optional<int64_t> Database::GetLastInsertId() {
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, "SELECT last_insert_rowid()", -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    std::optional<int64_t> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = sqlite3_column_int64(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return result;
}

std::string Database::HashPassword(const std::string& password) const {
    return userver::crypto::hash::Sha256(password);
}

bool Database::VerifyPassword(const std::string& password, const std::string& hash) const {
    return HashPassword(password) == hash;
}

std::optional<User> Database::CreateUser(const CreateUserRequest& request) {
    const char* sql = "INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    
    std::string password_hash = HashPassword(request.password);
    sqlite3_bind_text(stmt, 1, request.login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, request.email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, request.first_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, request.last_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, password_hash.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return std::nullopt;
    
    auto id = GetLastInsertId();
    return id ? FindUserById(*id) : std::nullopt;
}

std::optional<User> Database::FindUserByLogin(const std::string& login) {
    const char* sql = "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE login = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);
    
    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = sqlite3_column_int64(stmt, 0);
        u.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.first_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        u.last_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        u.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        result = u;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::optional<User> Database::FindUserById(int64_t id) {
    const char* sql = "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, id);
    
    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = sqlite3_column_int64(stmt, 0);
        u.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.first_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        u.last_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        u.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        result = u;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<User> Database::SearchUsersByNameMask(const std::string& mask) {
    std::string search = "%" + mask + "%";
    const char* sql = "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE first_name || ' ' || last_name LIKE ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return {};
    sqlite3_bind_text(stmt, 1, search.c_str(), -1, SQLITE_STATIC);
    
    std::vector<User> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = sqlite3_column_int64(stmt, 0);
        u.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.first_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        u.last_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        u.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        results.push_back(u);
    }
    sqlite3_finalize(stmt);
    return results;
}

std::optional<User> Database::AuthenticateUser(const std::string& login, const std::string& password) {
    auto u = FindUserByLogin(login);
    if (!u || !VerifyPassword(password, u->password_hash)) return std::nullopt;
    return u;
}

std::optional<Driver> Database::RegisterDriver(int64_t user_id, const RegisterDriverRequest& request) {
    const char* sql = "INSERT INTO drivers (user_id, full_name, license_plate) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, request.full_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, request.license_plate.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return std::nullopt;
    
    auto id = GetLastInsertId();
    return id ? GetDriverById(*id) : std::nullopt;
}

std::optional<Driver> Database::GetDriverByUserId(int64_t user_id) {
    const char* sql = "SELECT id, user_id, full_name, license_plate, status, created_at FROM drivers WHERE user_id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, user_id);
    std::optional<Driver> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Driver d;
        d.id = sqlite3_column_int64(stmt, 0);
        d.user_id = sqlite3_column_int64(stmt, 1);
        d.full_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        d.license_plate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        d.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        d.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
        result = d;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::optional<Driver> Database::GetDriverById(int64_t driver_id) {
    const char* sql = "SELECT id, user_id, full_name, license_plate, status, created_at FROM drivers WHERE id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, driver_id);
    std::optional<Driver> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Driver d;
        d.id = sqlite3_column_int64(stmt, 0);
        d.user_id = sqlite3_column_int64(stmt, 1);
        d.full_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        d.license_plate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        d.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        d.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
        result = d;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::optional<Ride> Database::CreateRide(int64_t passenger_id, const CreateRideRequest& request) {
    const char* sql = "INSERT INTO rides (passenger_id, from_address, to_address, cost) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, passenger_id);
    sqlite3_bind_text(stmt, 2, request.from_address.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, request.to_address.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, 10.0 + (rand() % 40)); // Random cost analogy
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return std::nullopt;
    
    auto id = GetLastInsertId();
    return id ? GetRideById(*id) : std::nullopt;
}

std::vector<Ride> Database::ListActiveRides() {
    const char* sql = "SELECT id, passenger_id, driver_id, from_address, to_address, status, cost, created_at FROM rides WHERE status != 'completed' ORDER BY created_at DESC";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return {};
    
    std::vector<Ride> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Ride r;
        r.id = sqlite3_column_int64(stmt, 0);
        r.passenger_id = sqlite3_column_int64(stmt, 1);
        if (sqlite3_column_type(stmt, 2) != SQLITE_NULL) r.driver_id = sqlite3_column_int64(stmt, 2);
        r.from_address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        r.to_address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        r.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        r.cost = sqlite3_column_double(stmt, 6);
        r.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
        results.push_back(r);
    }
    sqlite3_finalize(stmt);
    return results;
}

std::optional<Ride> Database::GetRideById(int64_t ride_id) {
    const char* sql = "SELECT id, passenger_id, driver_id, from_address, to_address, status, cost, created_at FROM rides WHERE id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, ride_id);
    std::optional<Ride> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Ride r;
        r.id = sqlite3_column_int64(stmt, 0);
        r.passenger_id = sqlite3_column_int64(stmt, 1);
        if (sqlite3_column_type(stmt, 2) != SQLITE_NULL) r.driver_id = sqlite3_column_int64(stmt, 2);
        r.from_address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        r.to_address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        r.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        r.cost = sqlite3_column_double(stmt, 6);
        r.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
        result = r;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Ride> Database::GetUserRideHistory(int64_t user_id) {
    const char* sql = "SELECT id, passenger_id, driver_id, from_address, to_address, status, cost, created_at FROM rides WHERE passenger_id = ? ORDER BY created_at DESC";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return {};
    sqlite3_bind_int64(stmt, 1, user_id);
    
    std::vector<Ride> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Ride r;
        r.id = sqlite3_column_int64(stmt, 0);
        r.passenger_id = sqlite3_column_int64(stmt, 1);
        if (sqlite3_column_type(stmt, 2) != SQLITE_NULL) r.driver_id = sqlite3_column_int64(stmt, 2);
        r.from_address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        r.to_address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        r.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        r.cost = sqlite3_column_double(stmt, 6);
        r.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
        results.push_back(r);
    }
    sqlite3_finalize(stmt);
    return results;
}

bool Database::AcceptRide(int64_t ride_id, int64_t driver_id) {
    const char* sql = "UPDATE rides SET driver_id = ?, status = 'accepted' WHERE id = ? AND status = 'pending'";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, driver_id);
    sqlite3_bind_int64(stmt, 2, ride_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::CompleteRide(int64_t ride_id) {
    const char* sql = "UPDATE rides SET status = 'completed' WHERE id = ? AND status = 'accepted'";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, ride_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

}  // namespace taxi_service
