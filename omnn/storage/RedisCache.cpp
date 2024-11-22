#include "RedisCache.h"
#include <chrono>
#include <thread>
#include <stdexcept>
#include <sstream>

using namespace omnn;
using namespace storage;

RedisCache::RedisCache(const std::string& host, int port, int timeout_ms,
                       int retry_count, int retry_delay_ms)
    : host_(host), port_(port), timeout_ms_(timeout_ms),
      retry_count_(retry_count), retry_delay_ms_(retry_delay_ms),
      context_(nullptr) {
    // Check if Redis is configured in environment
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        fprintf(stderr, "Redis host not configured in environment\n");
        return;  // Don't throw, just leave connection uninitialized
    }
    if (!Connect()) {
        std::ostringstream err;
        err << "Failed to connect to Redis server at " << host_ << ":" << port_;
        throw std::runtime_error(err.str());
    }
}

RedisCache::~RedisCache() {
    Disconnect();
}

bool RedisCache::Connect() {
    std::lock_guard<std::mutex> lock(mutex_);
    Disconnect();  // Ensure any existing connection is closed

    struct timeval timeout = {
        .tv_sec = timeout_ms_ / 1000,
        .tv_usec = (timeout_ms_ % 1000) * 1000
    };

    fprintf(stderr, "Attempting to connect to Redis at %s:%d (timeout: %dms)\n",
            host_.c_str(), port_, timeout_ms_);

    context_ = redisConnectWithTimeout(host_.c_str(), port_, timeout);

    if (context_ == nullptr || context_->err) {
        std::ostringstream err;
        err << "Redis connection error: ";
        if (context_) {
            err << context_->errstr << " (error code: " << context_->err << ")"
                << " - Details: ";
            switch(context_->err) {
                case REDIS_ERR_IO: err << "Network error"; break;
                case REDIS_ERR_EOF: err << "Connection closed"; break;
                case REDIS_ERR_PROTOCOL: err << "Protocol error"; break;
                case REDIS_ERR_OOM: err << "Out of memory"; break;
                case REDIS_ERR_OTHER: err << "Unknown error"; break;
                default: err << "Unrecognized error code";
            }
            redisFree(context_);
            context_ = nullptr;
        } else {
            err << "Could not allocate redis context";
        }
        fprintf(stderr, "%s\n", err.str().c_str());
        return false;
    }
    fprintf(stderr, "Successfully connected to Redis\n");
    return true;
}

void RedisCache::Disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (context_) {
        redisFree(context_);
        context_ = nullptr;
    }
}

bool RedisCache::RetryOperation(const std::function<bool()>& operation) {
    std::lock_guard<std::mutex> lock(mutex_);
    int current_delay = retry_delay_ms_;

    for (int i = 0; i < retry_count_; ++i) {
        if (operation()) {
            return true;
        }

        if (!IsConnected()) {
            fprintf(stderr, "Redis connection lost, attempt %d/%d to reconnect...\n", i + 1, retry_count_);
            if (!Connect()) {
                std::string error = context_ ? context_->errstr : "Connection failed";
                fprintf(stderr, "Reconnection failed: %s\n", error.c_str());
                std::this_thread::sleep_for(std::chrono::milliseconds(current_delay));
                current_delay = std::min(current_delay * 2, 5000);
                continue;
            }
        }

        fprintf(stderr, "Redis operation failed, retry attempt %d/%d (delay: %dms)\n",
                i + 1, retry_count_, current_delay);
        std::this_thread::sleep_for(std::chrono::milliseconds(current_delay));
        current_delay = std::min(current_delay * 2, 5000);
    }
    return false;
}

bool RedisCache::Set(const std::string_view& key, const std::string_view& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    return RetryOperation([this, &key, &value]() {
        if (!IsConnected()) return false;

        redisReply* reply = (redisReply*)redisCommand(context_, "SET %s %s",
                                                     key.data(), value.data());
        if (!reply) return false;

        bool success = (reply->type == REDIS_REPLY_STATUS &&
                       strcasecmp(reply->str, "OK") == 0);
        freeReplyObject(reply);
        return success;
    });
}

bool RedisCache::Get(const std::string_view& key, std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    return RetryOperation([this, &key, &value]() {
        if (!IsConnected()) return false;

        redisReply* reply = (redisReply*)redisCommand(context_, "GET %s",
                                                     key.data());
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

std::string RedisCache::GetOne(const std::string_view& key){
    std::string value;
    Get(key, value);
    return value;
}

bool RedisCache::Clear(const std::string_view& key) {
    std::lock_guard<std::mutex> lock(mutex_);
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
    std::lock_guard<std::mutex> lock(mutex_);
    return context_ && !context_->err;
}

std::string RedisCache::GetLastError() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!context_) {
        return "No Redis context available";
    }
    return context_->err ? context_->errstr : "";
}

std::string RedisCache::GetHiredisVersion() {
    return HIREDIS_VERSION;
}

bool RedisCache::IsVersionCompatible(const std::string& version, std::string* error_msg) {
    try {
        std::istringstream required(GetMinimumRequiredVersion()), actual(version);
        int req_major = 0, req_minor = 0, req_patch = 0;
        int ver_major = 0, ver_minor = 0, ver_patch = 0;
        char dot;

        required >> req_major >> dot >> req_minor >> dot >> req_patch;
        if (!(actual >> ver_major >> dot >> ver_minor >> dot >> ver_patch)) {
            if (error_msg) *error_msg = "Invalid version format: " + version;
            return false;
        }

        bool compatible = std::tie(ver_major, ver_minor, ver_patch) >=
                         std::tie(req_major, req_minor, req_patch);

        if (!compatible && error_msg) {
            std::ostringstream oss;
            oss << "Version " << version << " is below minimum required version "
                << GetMinimumRequiredVersion();
            *error_msg = oss.str();
        }
        return compatible;
    } catch (const std::exception& e) {
        if (error_msg) *error_msg = "Version check failed: " + std::string(e.what());
        return false;
    }
}

std::string RedisCache::GetVersion() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string version = "0.0.0";
    RetryOperation([&]() {
        if (!IsConnected()) {
            fprintf(stderr, "Cannot get Redis version: not connected\n");
            return false;
        }

        redisReply* reply = (redisReply*)redisCommand(context_, "INFO SERVER");
        if (!reply) {
            fprintf(stderr, "Failed to execute INFO SERVER command\n");
            return false;
        }

        bool success = false;
        if (reply->type == REDIS_REPLY_STRING) {
            std::string info(reply->str, reply->len);
            size_t pos = info.find("redis_version:");
            if (pos != std::string::npos) {
                pos += 13; // length of "redis_version:"
                size_t end = info.find("\r\n", pos);
                if (end != std::string::npos) {
                    std::string ver = info.substr(pos, end - pos);
                    if (std::count(ver.begin(), ver.end(), '.') >= 2) {
                        version = ver;
                        success = true;
                    } else {
                        fprintf(stderr, "Invalid version format: %s\n", ver.c_str());
                    }
                } else {
                    fprintf(stderr, "Malformed INFO response: missing line ending\n");
                }
            } else {
                fprintf(stderr, "Version information not found in INFO response\n");
            }
        } else {
            fprintf(stderr, "Unexpected reply type from INFO command: %d\n", reply->type);
        }
        freeReplyObject(reply);
        return success;
    });
    return version;
}
