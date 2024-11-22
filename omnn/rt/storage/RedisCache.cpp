#include "RedisCache.h"

#ifdef OPENMIND_STORAGE_REDIS

#include <hiredis/hiredis.h>
#include <stdexcept>
#include <cstdarg>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

namespace omnn::rt::storage {

RedisCache::RedisCache(const std::string_view& host, int port, int timeout_ms)
    : _context(nullptr, redisFree)
    , _host(host)
    , _port(port)
    , _timeout_ms(timeout_ms)
{
#ifdef _WIN32
    static bool wsaInitialized = false;
    if (!wsaInitialized) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("Failed to initialize Winsock");
        }
        wsaInitialized = true;
    }
#endif
    ensureConnection();
}

RedisCache::~RedisCache() {
#ifdef _WIN32
    WSACleanup();
#endif
}

void RedisCache::ensureConnection() {
    if (_context) return;

    const int retries = std::getenv("OPENMIND_TEST_REDIS_RETRY_COUNT") ?
        std::stoi(std::getenv("OPENMIND_TEST_REDIS_RETRY_COUNT")) : 5;
    const int delay_ms = std::getenv("OPENMIND_TEST_REDIS_RETRY_DELAY") ?
        std::stoi(std::getenv("OPENMIND_TEST_REDIS_RETRY_DELAY")) : 3000;
    std::string last_error;

    std::string platform_info;
#ifdef _WIN32
    platform_info = " on Windows";
#else
    platform_info = " on Unix";
#endif

#ifdef OPENMIND_STORAGE_REDIS_MEMURAI
    const char* server_type = "Memurai";
#else
    const char* server_type = "Redis";
#endif

    int attempts = retries;
    while (attempts--) {
        struct timeval timeout = { 0, _timeout_ms * 1000 };
        redisContext* c = redisConnectWithTimeout(_host.c_str(), _port, timeout);

        if (c && !c->err) {
            redisReply* reply = (redisReply*)redisCommand(c, "PING");
            if (reply && reply->type == REDIS_REPLY_STATUS &&
                std::string_view(reply->str, reply->len) == "PONG") {
                freeReplyObject(reply);
                _context.reset(c);
                return;
            }
            if (reply) {
                last_error = std::string(server_type) + " PING failed" + platform_info + ": " +
                    std::string(reply->str, reply->len);
                freeReplyObject(reply);
            } else {
                last_error = std::string(server_type) + " PING failed" + platform_info + ": no reply";
            }
            redisFree(c);
        } else {
            last_error = c ? c->errstr : "can't allocate redis context";
            if (c) redisFree(c);
        }

        if (attempts > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        }
    }

    throw std::runtime_error(std::string(server_type) + " connection error" + platform_info +
        " after " + std::to_string(retries) + " attempts to " + _host + ":" +
        std::to_string(_port) + ". Last error: " + last_error);
}

std::unique_ptr<redisReply, void(*)(redisReply*)> RedisCache::executeCommand(const char* format, ...) {
    ensureConnection();

    va_list ap;
    va_start(ap, format);
    redisReply* reply = (redisReply*)redisvCommand(_context.get(), format, ap);
    va_end(ap);

    if (!reply) {
        std::string err = _context ? _context->errstr : "unknown error";
        _context.reset();  // Reset connection on error to force reconnect
        throw std::runtime_error("Redis command failed: " + err);
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        std::string err(reply->str, reply->len);
        freeReplyObject(reply);
        throw std::runtime_error("Redis command error: " + err);
    }

    auto deleter = reinterpret_cast<void(*)(redisReply*)>(freeReplyObject);
    return std::unique_ptr<redisReply, void(*)(redisReply*)>(reply, deleter);
}

std::string RedisCache::GetOne(const std::string_view& key) {
    if (key.empty()) {
        throw std::invalid_argument("Key cannot be empty");
    }

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
    if (key.empty()) {
        throw std::invalid_argument("Key cannot be empty");
    }

    auto reply = executeCommand("SET %b %b", key.data(), key.size(), v.data(), v.size());
    return reply->type == REDIS_REPLY_STATUS && std::string_view(reply->str, reply->len) == "OK";
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
