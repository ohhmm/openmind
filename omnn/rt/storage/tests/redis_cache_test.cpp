#define BOOST_TEST_MODULE redis_cache_test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../CacheBase.h"
#include "../RedisCache.h"
#include <chrono>
#include <thread>
#include <cstdlib>
#include <boost/test/unit_test_monitor.hpp>

#ifdef OPENMIND_STORAGE_REDIS

BOOST_AUTO_TEST_SUITE(redis_cache_suite)

using namespace omnn::rt::storage;

namespace {
bool is_redis_available() {
    const char* host = std::getenv("OPENMIND_TEST_REDIS_HOST") ?
        std::getenv("OPENMIND_TEST_REDIS_HOST") : "127.0.0.1";
    const int port = std::getenv("OPENMIND_TEST_REDIS_PORT") ?
        std::stoi(std::getenv("OPENMIND_TEST_REDIS_PORT")) : 6379;
    const int retries = std::getenv("OPENMIND_TEST_REDIS_RETRY_COUNT") ?
        std::stoi(std::getenv("OPENMIND_TEST_REDIS_RETRY_COUNT")) : 5;
    const int delay_ms = std::getenv("OPENMIND_TEST_REDIS_RETRY_DELAY") ?
        std::stoi(std::getenv("OPENMIND_TEST_REDIS_RETRY_DELAY")) : 2000;
    std::string last_error;

    std::string server_info;
#ifdef OPENMIND_STORAGE_REDIS_MEMURAI
    server_info = "Memurai Developer";
    BOOST_TEST_MESSAGE("Testing with Memurai Developer at " + std::string(host) + ":" + std::to_string(port));
#else
    server_info = "Redis Server";
    BOOST_TEST_MESSAGE("Testing with Redis Server at " + std::string(host) + ":" + std::to_string(port));
#endif

    int attempt = 1;
    while (attempt <= retries) {
        try {
            RedisCache test_cache(host, port);
            BOOST_TEST_MESSAGE(server_info + " connection successful after " +
                std::to_string(attempt) + " attempt(s)");
            return true;
        } catch (const std::exception& e) {
            last_error = e.what();
            BOOST_TEST_MESSAGE(server_info + " connection attempt " +
                std::to_string(attempt) + "/" + std::to_string(retries) +
                " failed: " + last_error);

            if (attempt < retries) {
                BOOST_TEST_MESSAGE("Waiting " + std::to_string(delay_ms) + "ms before next attempt...");
                std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
            }
            attempt++;
        }
    }

    BOOST_TEST_MESSAGE(server_info + " connection failed after " + std::to_string(retries) +
        " attempts at " + std::string(host) + ":" + std::to_string(port) +
        ". Last error: " + last_error);
    return false;
}
}

BOOST_AUTO_TEST_CASE(redis_cache_basic_operations) {
    if (!is_redis_available()) {
        BOOST_TEST_MESSAGE("Redis/Memurai server is not available - skipping test");
        return;
    }

    RedisCache cache;  // Use default constructor for platform-specific settings

    // Test Set and GetOne
    BOOST_CHECK(cache.Set("test_key", "test_value"));
    BOOST_CHECK_EQUAL(cache.GetOne("test_key"), "test_value");

    // Test Clear
    BOOST_CHECK(cache.Clear("test_key"));
    BOOST_CHECK_EQUAL(cache.GetOne("test_key"), "");

    // Test ResetAllDB
    BOOST_CHECK(cache.Set("key1", "value1"));
    BOOST_CHECK(cache.Set("key2", "value2"));
    BOOST_CHECK(cache.ResetAllDB(CacheBase::path_str_t("")));
    BOOST_CHECK_EQUAL(cache.GetOne("key1"), "");
    BOOST_CHECK_EQUAL(cache.GetOne("key2"), "");
}

BOOST_AUTO_TEST_CASE(redis_cache_error_handling) {
    if (!is_redis_available()) {
        BOOST_TEST_MESSAGE("Redis/Memurai server is not available - skipping test");
        return;
    }

    // Test connection error handling with environment variables
    const char* test_host = std::getenv("OPENMIND_TEST_REDIS_HOST") ? std::getenv("OPENMIND_TEST_REDIS_HOST") : "127.0.0.1";
    BOOST_CHECK_THROW(RedisCache("nonexistent.invalid", std::stoi(std::getenv("OPENMIND_TEST_REDIS_PORT") ? std::getenv("OPENMIND_TEST_REDIS_PORT") : "6379")), std::runtime_error);

    RedisCache cache;

    // Test invalid key operations
    BOOST_CHECK_THROW(cache.GetOne(""), std::invalid_argument);
    BOOST_CHECK_THROW(cache.Set("", "value"), std::invalid_argument);

    // Test large value handling
    std::string large_value(1024 * 1024, 'x');  // 1MB string
    BOOST_CHECK(cache.Set("large_key", large_value));
    BOOST_CHECK_EQUAL(cache.GetOne("large_key"), large_value);

#ifdef OPENMIND_STORAGE_REDIS_MEMURAI
    BOOST_TEST_MESSAGE("Running Memurai-specific error handling tests");
#else
    BOOST_TEST_MESSAGE("Running Redis-specific error handling tests");
#endif
}

BOOST_AUTO_TEST_SUITE_END()

#endif // OPENMIND_STORAGE_REDIS

#ifdef BOOST_TEST_DYN_LINK
bool init_unit_test() {
    boost::unit_test::framework::master_test_suite().p_name.value = "redis_cache_test";
    return true;
}

int main(int argc, char* argv[]) {
    return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
#endif