#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

#include <userver/storages/redis/client.hpp>

namespace taxi_service {

struct RateLimitResult {
    bool allowed;
    int64_t limit;
    int64_t remaining;
    int64_t reset_ts;  ///< Unix timestamp of window expiry
};

/// Sliding-window counter rate limiter backed by Redis.
///
/// Algorithm:
///   window_key = "rl:" + key + ":" + (now_sec / window_sec)
///   count = INCR window_key
///   if count == 1: EXPIRE window_key window_sec
///   allowed = count <= limit
///
/// Thread-safe.
class RateLimiter {
public:
    explicit RateLimiter(userver::storages::redis::ClientPtr redis);

    /// @param key     Unique rate-limit identifier (e.g. "login:<ip>")
    /// @param limit   Max requests per window
    /// @param window  Window duration
    RateLimitResult Check(const std::string& key, int64_t limit,
                          std::chrono::seconds window);

private:
    userver::storages::redis::ClientPtr redis_;
};

}  // namespace taxi_service
