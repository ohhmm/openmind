#define BOOST_TEST_MODULE redis_cache_test
#include <boost/test/unit_test.hpp>
#include "../RedisCache.h"

#ifdef OPENMIND_STORAGE_REDIS

using namespace omnn::rt::storage;

struct RedisFixture {
    bool redis_available;

    RedisFixture() {
        // Try to connect to Redis
        try {
            RedisCache test_cache("localhost", 6379);
            redis_available = true;
        } catch (const std::runtime_error&) {
            redis_available = false;
            BOOST_TEST_MESSAGE("Redis server is not available - skipping tests");
        }
    }
};

BOOST_FIXTURE_TEST_SUITE(redis_cache_tests, RedisFixture)

BOOST_AUTO_TEST_CASE(redis_cache_basic_operations) {
    if (!redis_available) return;

    RedisCache cache("localhost", 6379);

    // Test Set and GetOne
    BOOST_CHECK(cache.Set("test_key", "test_value"));
    BOOST_CHECK_EQUAL(cache.GetOne("test_key"), "test_value");

    // Test Clear
    BOOST_CHECK(cache.Clear("test_key"));
    BOOST_CHECK_EQUAL(cache.GetOne("test_key"), "");

    // Test ResetAllDB
    BOOST_CHECK(cache.Set("key1", "value1"));
    BOOST_CHECK(cache.Set("key2", "value2"));
    BOOST_CHECK(cache.ResetAllDB(""));
    BOOST_CHECK_EQUAL(cache.GetOne("key1"), "");
    BOOST_CHECK_EQUAL(cache.GetOne("key2"), "");
}

BOOST_AUTO_TEST_CASE(redis_cache_error_handling) {
    if (!redis_available) return;

    // Test connection to non-existent Redis server
    BOOST_CHECK_THROW(RedisCache("nonexistent", 6379), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()

#endif // OPENMIND_STORAGE_REDIS
