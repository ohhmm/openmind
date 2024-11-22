#pragma once

#ifdef _WIN32
#include <winsock2.h>  // Required by hiredis on Windows
#endif

#include "CacheBase.h"

#ifdef OPENMIND_STORAGE_REDIS

#include <memory>
#include <string>
#include <string_view>

struct redisContext;  // Forward declaration
struct redisReply;    // Forward declaration

namespace omnn::rt::storage {

class RedisCache : public CacheBase {
    std::unique_ptr<redisContext, void(*)(redisContext*)> _context;
    const std::string _host;
    const int _port;
    const int _timeout_ms;
    bool _debug_logging;

public:
    RedisCache(const std::string_view& host = "localhost",
               int port = 6379,  // Default port for both Redis and Memurai
               int timeout_ms = 5000);  // Increased from 1000ms to 5000ms for CI environments
    ~RedisCache() override;

    std::string GetOne(const std::string_view& key) override;
    bool Set(const std::string_view& key, const std::string_view& v) override;
    bool Clear(const std::string_view& key) override;
    bool ResetAllDB(const CacheBase::path_str_t& path) override;

private:
    void ensureConnection();
    std::unique_ptr<redisReply, void(*)(redisReply*)> executeCommand(const char* format, ...);
    void logDebug(const std::string& message) const;
    bool isDebugLogging() const { return _debug_logging; }
};

} // namespace omnn::rt::storage

#endif // OPENMIND_STORAGE_REDIS
