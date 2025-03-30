#include "RedisCache.h"
#include <chrono>
#include <thread>
#include <stdexcept>

namespace omnn {
namespace rt {
namespace storage {

RedisCache::RedisCache(const std::string& host, int port, int timeout_ms,
                       int retry_count, int retry_delay_ms)
    : host_(host), port_(port), timeout_ms_(timeout_ms),
      retry_count_(retry_count), retry_delay_ms_(retry_delay_ms),
      context_(nullptr) {
    if (!Connect()) {
        throw std::runtime_error("Failed to connect to Redis server");
    }
}

RedisCache::~RedisCache() {
    Disconnect();
}

bool RedisCache::Connect() {
    Disconnect();  // Ensure any existing connection is closed

    struct timeval timeout = {
        .tv_sec = timeout_ms_ / 1000,
        .tv_usec = (timeout_ms_ % 1000) * 1000
    };

    context_ = redisConnectWithTimeout(host_.c_str(), port_, timeout);

    if (context_ == nullptr || context_->err) {
        if (context_) {
            redisFree(context_);
            context_ = nullptr;
        }
        return false;
    }
    return true;
}

void RedisCache::Disconnect() {
    if (context_) {
        redisFree(context_);
        context_ = nullptr;
    }
}

bool RedisCache::RetryOperation(const std::function<bool()>& operation) {
    for (int i = 0; i < retry_count_; ++i) {
        if (operation()) {
            return true;
        }

        if (!IsConnected() && !Connect()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay_ms_));
            continue;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay_ms_));
    }
    return false;
}

bool RedisCache::Set(const std::string& key, const std::string& value) {
    return RetryOperation([this, &key, &value]() {
        if (!IsConnected()) return false;

        redisReply* reply = (redisReply*)redisCommand(context_, "SET %s %s",
                                                     key.c_str(), value.c_str());
        if (!reply) return false;

        bool success = (reply->type == REDIS_REPLY_STATUS &&
                       strcasecmp(reply->str, "OK") == 0);
        freeReplyObject(reply);
        return success;
    });
}

bool RedisCache::Get(const std::string& key, std::string& value) {
    return RetryOperation([this, &key, &value]() {
        if (!IsConnected()) return false;

        redisReply* reply = (redisReply*)redisCommand(context_, "GET %s",
                                                     key.c_str());
        if (!reply) return false;

        bool success = false;
        if (reply->type == REDIS_REPLY_STRING) {
            value = std::string(reply->str, reply->len);
            success = true;
        }

        freeReplyObject(reply);
        return success;
    });
}

bool RedisCache::Clear() {
    return RetryOperation([this]() {
        if (!IsConnected()) return false;

        redisReply* reply = (redisReply*)redisCommand(context_, "FLUSHDB");
        if (!reply) return false;

        bool success = (reply->type == REDIS_REPLY_STATUS &&
                       strcasecmp(reply->str, "OK") == 0);
        freeReplyObject(reply);
        return success;
    });
}

bool RedisCache::IsConnected() const {
    return context_ && !context_->err;
}

} // namespace storage
} // namespace rt
} // namespace omnn
