#define BOOST_TEST_MODULE redis_cache_test
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include "../CacheBase.h"
#include "../RedisCache.h"
#include <chrono>
#include <thread>

#ifdef OPENMIND_STORAGE_REDIS

BOOST_AUTO_TEST_SUITE(redis_cache_suite)

using namespace omnn::rt::storage;

namespace {
bool is_redis_available() {
    int retries = 5;
    std::string last_error;
#ifdef OPENMIND_STORAGE_REDIS_MEMURAI
    const char* server_type = "Memurai";
#else
    const char* server_type = "Redis";
#endif
    while (retries--) {
        try {
            RedisCache test_cache;  // Use default constructor for platform-specific defaults
            return true;
        } catch (const std::exception& e) {
            last_error = e.what();
            if (retries > 0) {
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
    }
    BOOST_TEST_MESSAGE(std::string(server_type) + " connection failed after retries. Last error: " + last_error);
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

    // Test connection to non-existent server
    BOOST_CHECK_THROW(RedisCache("nonexistent.local", 6379), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()

#endif // OPENMIND_STORAGE_REDIS

bool init_unit_test() {
    return true;
}

int main(int argc, char* argv[]) {
    return boost::unit_test::unit_test_main(init_unit_test, argc, argv);
}
