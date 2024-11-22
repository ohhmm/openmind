#define BOOST_TEST_MODULE redis_cache_test
#include <boost/test/unit_test.hpp>
#include "../RedisCache.h"

#ifdef OPENMIND_STORAGE_REDIS

using namespace omnn::rt::storage;

BOOST_AUTO_TEST_CASE(redis_cache_basic_operations) {
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
    // Test connection to non-existent Redis server
    BOOST_CHECK_THROW(RedisCache("nonexistent", 6379), std::runtime_error);
}

#endif // OPENMIND_STORAGE_REDIS
