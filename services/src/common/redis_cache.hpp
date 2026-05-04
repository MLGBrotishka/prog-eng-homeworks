#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <string>

#include <userver/storages/redis/client.hpp>

namespace taxi_service {

/// Cache-Aside wrapper around the userver Redis client.
///
/// Provides typed Get/Set/Del operations with TTL.
/// Thread-safe: the underlying ClientPtr is itself thread-safe.
class RedisCache {
public:
    explicit RedisCache(userver::storages::redis::ClientPtr redis);

    /// Returns cached value, or std::nullopt on miss.
    std::optional<std::string> Get(const std::string& key) const;

    /// Stores value with the given TTL (SETEX semantics).
    void Set(const std::string& key, const std::string& value,
             std::chrono::seconds ttl);

    /// Deletes a key (no-op if key does not exist).
    void Del(const std::string& key);

private:
    userver::storages::redis::ClientPtr redis_;
};

}  // namespace taxi_service
