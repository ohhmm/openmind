#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <functional>
#include <hiredis/hiredis.h>
#include "CacheBase.h"

#ifndef REDIS_HOST
#define REDIS_HOST "localhost"
#endif

#ifndef REDIS_PORT
#define REDIS_PORT 6379
#endif

namespace omnn {
namespace rt {
namespace storage {

class RedisCache : public CacheBase {
public:
    RedisCache(const std::string& host = "", int port = 0,
               int timeout_ms = 1000, int retry_count = 5, int retry_delay_ms = 1000);
    ~RedisCache();

    bool Set(const std::string& key, const std::string& value) override;
    bool Set(const std::string& key, const std::string& value, int expire_seconds);
    bool Get(const std::string& key, std::string& value) override;
    bool Clear() override;
    bool IsConnected() const;
    bool Pipeline(const std::vector<std::pair<std::string, std::string>>& operations);
    bool PipelineWithExpire(const std::vector<std::pair<std::string, std::string>>& operations, int expire_seconds);

    // Binary data handling
    bool SetBinary(const std::string& key, const void* data, size_t size);
    bool GetBinary(const std::string& key, std::vector<char>& data);

    // For testing purposes only
    redisContext* GetContext() const { return context_; }

private:
    bool Connect();
    void Disconnect();
    bool RetryOperation(const std::function<bool()>& operation);

    std::string host_;
    int port_;
    int timeout_ms_;
    int retry_count_;
    int retry_delay_ms_;
    redisContext* context_;
};

} // namespace storage
} // namespace rt
} // namespace omnn
