#include "rate_limiter.hpp"

#include <chrono>
#include <stdexcept>

#include <userver/logging/log.hpp>
#include <userver/storages/redis/command_options.hpp>

namespace taxi_service {

RateLimiter::RateLimiter(userver::storages::redis::ClientPtr redis)
    : redis_(std::move(redis)) {}

RateLimitResult RateLimiter::Check(const std::string& key, int64_t limit,
                                   std::chrono::seconds window) {
    const int64_t window_sec = window.count();
    const int64_t now_sec =
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    const int64_t window_ts = now_sec / window_sec;
    const int64_t reset_ts = (window_ts + 1) * window_sec;

    const std::string redis_key = "rl:" + key + ":" + std::to_string(window_ts);

    try {
        const int64_t count = redis_->Incr(redis_key, {}).Get();

        // Only set expiry on first increment to avoid races and extra RTTs.
        if (count == 1) {
            redis_->Expire(redis_key, window, {}).Get();
        }

        const int64_t remaining =
            (count <= limit) ? (limit - count) : 0;
        const bool allowed = (count <= limit);

        LOG_DEBUG() << "RateLimit key=" << redis_key << " count=" << count
                    << " limit=" << limit << " allowed=" << allowed;

        return {allowed, limit, remaining, reset_ts};
    } catch (const std::exception& ex) {
        // On Redis failure, allow the request (fail-open policy).
        LOG_WARNING() << "RateLimiter Redis error key=" << redis_key
                      << ": " << ex.what() << ". Failing open.";
        return {true, limit, 0, reset_ts};
    }
}

}  // namespace taxi_service
