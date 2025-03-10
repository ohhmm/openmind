#pragma once

#include <string>
#include <memory>
#include <hiredis/hiredis.h>
#include "CacheBase.h"

namespace omnn {
namespace rt {
namespace storage {

class RedisCache : public CacheBase {
public:
    RedisCache(const std::string& host = "localhost", int port = 6379,
               int timeout_ms = 1000, int retry_count = 5, int retry_delay_ms = 1000);
    ~RedisCache();

    bool Set(const std::string& key, const std::string& value) override;
    bool Get(const std::string& key, std::string& value) override;
    bool Clear() override;
    bool IsConnected() const;

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
