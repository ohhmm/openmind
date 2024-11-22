#include "RedisCache.h"

#ifdef OPENMIND_STORAGE_REDIS

#include <hiredis/hiredis.h>
#include <stdexcept>
#include <cstdarg>
#include <chrono>
#include <thread>

namespace omnn::rt::storage {

RedisCache::RedisCache(const std::string_view& host, int port, int timeout_ms)
    : _context(nullptr, redisFree)
    , _host(host)
    , _port(port)
    , _timeout_ms(timeout_ms)
{
    ensureConnection();
}

RedisCache::~RedisCache() = default;

void RedisCache::ensureConnection() {
    if (_context) return;

    int retries = 3;
    std::string last_error;

    while (retries--) {
        struct timeval timeout = { 0, _timeout_ms * 1000 };
        redisContext* c = redisConnectWithTimeout(_host.c_str(), _port, timeout);

        if (c && !c->err) {
            _context.reset(c);
            return;
        }

        last_error = c ? c->errstr : "can't allocate redis context";
        if (c) redisFree(c);

        if (retries > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    throw std::runtime_error("Redis connection error after retries: " + last_error);
}

std::unique_ptr<redisReply, void(*)(redisReply*)> RedisCache::executeCommand(const char* format, ...) {
    ensureConnection();

    va_list ap;
    va_start(ap, format);
    redisReply* reply = (redisReply*)redisvCommand(_context.get(), format, ap);
    va_end(ap);

    if (!reply) {
        throw std::runtime_error("Redis command failed");
    }

    // Cast freeReplyObject to the correct function pointer type
    auto deleter = reinterpret_cast<void(*)(redisReply*)>(freeReplyObject);
    return std::unique_ptr<redisReply, void(*)(redisReply*)>(reply, deleter);
}

std::string RedisCache::GetOne(const std::string_view& key) {
    auto reply = executeCommand("GET %b", key.data(), key.size());

    if (reply->type == REDIS_REPLY_NIL) {
        return "";
    }

    if (reply->type != REDIS_REPLY_STRING) {
        throw std::runtime_error("Unexpected Redis reply type for GET");
    }

    return std::string(reply->str, reply->len);
}

bool RedisCache::Set(const std::string_view& key, const std::string_view& v) {
    auto reply = executeCommand("SET %b %b", key.data(), key.size(), v.data(), v.size());

    return reply->type == REDIS_REPLY_STATUS &&
           std::string_view(reply->str, reply->len) == "OK";
}

bool RedisCache::Clear(const std::string_view& key) {
    auto reply = executeCommand("DEL %b", key.data(), key.size());

    return reply->type == REDIS_REPLY_INTEGER && reply->integer > 0;
}

bool RedisCache::ResetAllDB(const CacheBase::path_str_t& path) {
    auto reply = executeCommand("FLUSHDB");

    return reply->type == REDIS_REPLY_STATUS &&
           std::string_view(reply->str, reply->len) == "OK";
}

} // namespace omnn::rt::storage

#endif // OPENMIND_STORAGE_REDIS
