#include "RedisCache.h"
#include <chrono>
#include <thread>
#include <stdexcept>

namespace omnn {
namespace rt {
namespace storage {

RedisCache::RedisCache(const std::string& host, int port, int timeout_ms,
                       int retry_count, int retry_delay_ms)
    : host_(host.empty() ? REDIS_HOST : host),
      port_(port == 0 ? REDIS_PORT : port),
      timeout_ms_(timeout_ms),
      retry_count_(retry_count),
      retry_delay_ms_(retry_delay_ms),
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

    if (context_ == nullptr) {
        return false;
    }

    if (context_->err) {
        std::string error = context_->errstr ? context_->errstr : "Unknown error";
        redisFree(context_);
        context_ = nullptr;
        return false;
    }

    // Set socket timeout for operations
    if (redisSetTimeout(context_, timeout) != REDIS_OK) {
        redisFree(context_);
        context_ = nullptr;
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

        // If connection is lost, attempt to reconnect
        if (!IsConnected()) {
            Disconnect();  // Ensure cleanup before reconnect
            if (Connect()) {
                continue;  // Retry immediately after successful reconnect
            }
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

bool RedisCache::Set(const std::string& key, const std::string& value, int expire_seconds) {
    return RetryOperation([this, &key, &value, expire_seconds]() {
        if (!IsConnected()) return false;

        redisReply* reply = (redisReply*)redisCommand(context_, "SET %s %s",
                                                    key.c_str(), value.c_str());
        if (!reply) return false;

        bool success = (reply->type == REDIS_REPLY_STATUS &&
                       strcasecmp(reply->str, "OK") == 0);
        freeReplyObject(reply);

        if (success) {
            reply = (redisReply*)redisCommand(context_, "EXPIRE %s %d",
                                           key.c_str(), expire_seconds);
            if (reply) {
                success = (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1);
                freeReplyObject(reply);
            } else {
                success = false;
            }
        }
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

bool RedisCache::SetBinary(const std::string& key, const void* data, size_t size) {
    if (!data && size > 0) return false;
    return RetryOperation([this, &key, data, size]() {
        if (!IsConnected()) return false;
        redisReply* reply = (redisReply*)redisCommand(context_, "SET %s %b", key.c_str(), data ? data : "", size);
        if (!reply) return false;
        bool success = (reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str, "OK") == 0);
        freeReplyObject(reply);
        return success;
    });
}

bool RedisCache::PipelineWithExpire(const std::vector<std::pair<std::string, std::string>>& operations, int expire_seconds) {
    return RetryOperation([this, &operations, expire_seconds]() {
        if (!IsConnected()) return false;

        // Pipeline SET and EXPIRE commands
        for (const auto& op : operations) {
            redisAppendCommand(context_, "SET %s %s", op.first.c_str(), op.second.c_str());
            redisAppendCommand(context_, "EXPIRE %s %d", op.first.c_str(), expire_seconds);
        }

        // Execute pipeline
        bool success = true;
        for (size_t i = 0; i < operations.size() * 2; ++i) {
            redisReply* reply;
            if (redisGetReply(context_, (void**)&reply) != REDIS_OK) {
                success = false;
                break;
            }
            bool cmd_success = (i % 2 == 0) ?
                (reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str, "OK") == 0) :
                (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1);
            freeReplyObject(reply);
            if (!cmd_success) {
                success = false;
                break;
            }
        }
        return success;
    });
}

bool RedisCache::Pipeline(const std::vector<std::pair<std::string, std::string>>& operations) {
    return RetryOperation([this, &operations]() {
        if (!IsConnected()) return false;

        // Start pipeline
        for (const auto& op : operations) {
            redisAppendCommand(context_, "SET %s %s",
                             op.first.c_str(), op.second.c_str());
        }

        // Execute pipeline
        bool success = true;
        for (size_t i = 0; i < operations.size(); ++i) {
            redisReply* reply;
            if (redisGetReply(context_, (void**)&reply) != REDIS_OK) {
                success = false;
                break;
            }
            bool cmd_success = (reply->type == REDIS_REPLY_STATUS &&
                              strcasecmp(reply->str, "OK") == 0);
            freeReplyObject(reply);
            if (!cmd_success) {
                success = false;
                break;
            }
        }
        return success;
    });
}

bool RedisCache::GetBinary(const std::string& key, std::vector<char>& data) {
    return RetryOperation([this, &key, &data]() {
        if (!IsConnected()) return false;
        redisReply* reply = (redisReply*)redisCommand(context_, "GET %s", key.c_str());
        if (!reply) return false;
        bool success = false;
        if (reply->type == REDIS_REPLY_STRING) {
            data.assign(reply->str, reply->str + reply->len);
            success = true;
        }
        freeReplyObject(reply);
        return success;
    });
}

} // namespace storage
} // namespace rt
} // namespace omnn
