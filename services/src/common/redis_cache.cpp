#include "redis_cache.hpp"

#include <userver/logging/log.hpp>
#include <userver/storages/redis/command_options.hpp>

namespace taxi_service {

RedisCache::RedisCache(userver::storages::redis::ClientPtr redis)
    : redis_(std::move(redis)) {}

std::optional<std::string> RedisCache::Get(const std::string& key) const {
    try {
        auto result = redis_->Get(key, {}).Get();
        if (result) {
            LOG_DEBUG() << "Redis cache HIT key=" << key;
        } else {
            LOG_DEBUG() << "Redis cache MISS key=" << key;
        }
        return result;
    } catch (const std::exception& ex) {
        LOG_WARNING() << "Redis GET failed key=" << key << ": " << ex.what();
        return std::nullopt;
    }
}

void RedisCache::Set(const std::string& key, const std::string& value,
                     std::chrono::seconds ttl) {
    try {
        // Setex takes (key, seconds, value, command_control)
        redis_->Setex(key, ttl, value, {}).Get();
        LOG_DEBUG() << "Redis cache SET key=" << key
                    << " ttl=" << ttl.count() << "s";
    } catch (const std::exception& ex) {
        LOG_WARNING() << "Redis SETEX failed key=" << key << ": " << ex.what();
    }
}

void RedisCache::Del(const std::string& key) {
    try {
        redis_->Del(key, {}).Get();
        LOG_DEBUG() << "Redis cache DEL key=" << key;
    } catch (const std::exception& ex) {
        LOG_WARNING() << "Redis DEL failed key=" << key << ": " << ex.what();
    }
}

}  // namespace taxi_service
