#pragma once
#include "CacheBase.h"

#ifdef OPENMIND_STORAGE_REDIS

#include <memory>
#include <string>
#include <string_view>

struct redisContext;  // Forward declaration
struct redisReply;    // Forward declaration

namespace omnn::rt::storage {

using path_str_t = boost::filesystem::path;

class RedisCache : public CacheBase {
    std::unique_ptr<redisContext, void(*)(redisContext*)> _context;
    const std::string _host;
    const int _port;
    const int _timeout_ms;

public:
    RedisCache(const std::string_view& host = "localhost",
               int port = 6379,
               int timeout_ms = 1000);
    ~RedisCache() override;

    std::string GetOne(const std::string_view& key) override;
    bool Set(const std::string_view& key, const std::string_view& v) override;
    bool Clear(const std::string_view& key) override;
    bool ResetAllDB(const path_str_t& path) override;

private:
    void ensureConnection();
    std::unique_ptr<redisReply, void(*)(redisReply*)> executeCommand(const char* format, ...);
};

} // namespace omnn::rt::storage

#endif // OPENMIND_STORAGE_REDIS
