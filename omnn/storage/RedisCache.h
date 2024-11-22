#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <hiredis/hiredis.h>
#include "CacheBase.h"

namespace omnn::storage {

class RedisCache : public CacheBase {
public:
    RedisCache(const std::string& host = "localhost", int port = 6379,
               int timeout_ms = 1000, int retry_count = 5, int retry_delay_ms = 1000);
    ~RedisCache();

    bool Set(const std::string_view& key, const std::string_view& value) override;
    std::string GetOne(const std::string_view& key) override;

    bool Get(const std::string_view& key, std::string& value);

    bool Clear(const std::string_view& key = "") override;

    /// @brief Check if the Redis connection is active
    /// @return true if connected, false otherwise
    bool IsConnected() const;

    /// @brief Get the version of the connected Redis server
    /// @return Version string in format "X.Y.Z", or "0.0.0" if version cannot be determined
    std::string GetVersion();

    /// @brief Check if the given Redis version meets minimum requirements
    /// @param version Version string in format "X.Y.Z"
    /// @param error_msg Optional output parameter to receive detailed error message
    /// @return true if version is compatible (>= 0.13.0), false otherwise
    static bool IsVersionCompatible(const std::string& version, std::string* error_msg = nullptr);

    /// @brief Get the minimum required Redis version
    /// @return Version string in format "X.Y.Z"
    static std::string GetMinimumRequiredVersion() { return "0.13.0"; }

    /// @brief Get the last error message from Redis operations
    /// @return Error message string or empty if no error
    std::string GetLastError() const;

    /// @brief Get the hiredis library version
    /// @return Version string of the hiredis library
    static std::string GetHiredisVersion();

    /// @brief Get the hiredis library version
    /// @return Version string of the hiredis library
    constexpr auto GetRetryCount() const { return retry_count_; }
    constexpr auto GetRetryDelay() const { return retry_delay_ms_; }

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
    mutable std::string last_error_;
    mutable std::mutex mutex_; // Protect concurrent access to context and error state
};

} // namespace omnn::storage
