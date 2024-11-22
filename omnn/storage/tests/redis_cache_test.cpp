#define BOOST_TEST_MODULE redis_cache_test
#include <boost/test/unit_test.hpp>
#include "../RedisCache.h"
#include <cstdlib>
#include <string>
#include <thread>

using namespace omnn::storage;

struct RedisTestConfig {
    RedisTestConfig() {
        // Get configuration from environment
        const char* host = std::getenv("OPENMIND_TEST_REDIS_HOST");
        const char* port = std::getenv("OPENMIND_TEST_REDIS_PORT");
        const char* retry_count = std::getenv("OPENMIND_TEST_REDIS_RETRY_COUNT");
        const char* retry_delay = std::getenv("OPENMIND_TEST_REDIS_RETRY_DELAY");

        host_ = host ? host : "localhost";
        port_ = port ? std::stoi(port) : 6379;
        retry_count_ = retry_count ? std::stoi(retry_count) : 5;
        retry_delay_ms_ = retry_delay ? std::stoi(retry_delay) : 1000;

        // Log test configuration
        BOOST_TEST_MESSAGE("Redis Test Configuration:");
        BOOST_TEST_MESSAGE("  Host: " + host_);
        BOOST_TEST_MESSAGE("  Port: " + std::to_string(port_));
        BOOST_TEST_MESSAGE("  Retry Count: " + std::to_string(retry_count_));
        BOOST_TEST_MESSAGE("  Retry Delay: " + std::to_string(retry_delay_ms_) + "ms");
    }

    std::string host_;
    int port_;
    int retry_count_;
    int retry_delay_ms_;
};

BOOST_FIXTURE_TEST_SUITE(redis_cache_tests, RedisTestConfig)

// This test runs first to verify Redis service availability
BOOST_AUTO_TEST_CASE(test_redis_service_health, *boost::unit_test::description("Redis Service Health Check")) {
    const char* host = std::getenv("OPENMIND_TEST_REDIS_HOST");
    BOOST_TEST_MESSAGE("Redis Service Health Check:");
    BOOST_TEST_MESSAGE("  Environment Host: " + std::string(host ? host : "not set"));

    RedisCache cache(host ? host : "localhost", 6379, 1000, 3, 100);
    std::string version = cache.GetVersion();

    // Enhanced diagnostics
    BOOST_TEST_MESSAGE("Redis Server Information:");
    BOOST_TEST_MESSAGE("  Version: " + version);
    BOOST_TEST_MESSAGE("  Connection Status: " + std::string(cache.IsConnected() ? "Connected" : "Not Connected"));
    BOOST_TEST_MESSAGE("  Connection Pool Status:");
    BOOST_TEST_MESSAGE("    Max Retries: " << cache.GetRetryCount());
    BOOST_TEST_MESSAGE("    Retry Delay: " << cache.GetRetryDelay() << "ms");
    BOOST_TEST_MESSAGE("    Timeout: 1000ms");

    std::string error = cache.GetLastError();
    if (!error.empty()) {
        BOOST_TEST_MESSAGE("  Last Error: " + error);
    }

    if (!host) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }

    BOOST_CHECK_MESSAGE(cache.IsConnected(),
        "Redis service is not available. Check Redis server status and connection settings.");
    BOOST_CHECK_MESSAGE(version != "0.0.0",
        "Redis version could not be determined. Check if Redis server is responding correctly.");
}

BOOST_AUTO_TEST_CASE(test_basic_operations) {
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }

    RedisCache cache(host_, port_, 1000, retry_count_, retry_delay_ms_);

    // Test Set operation
    BOOST_CHECK(cache.Set("test_key", "test_value"));

    // Test Get operation
    std::string value;
    BOOST_CHECK(cache.Get("test_key", value));
    BOOST_CHECK_EQUAL(value, "test_value");

    // Test Clear operation
    BOOST_CHECK(cache.Clear());
    BOOST_CHECK(!cache.Get("test_key", value));
}

BOOST_AUTO_TEST_CASE(test_nonexistent_key) {
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }
    RedisCache cache(host_, port_, 1000, retry_count_, retry_delay_ms_);

    std::string value;
    BOOST_CHECK(!cache.Get("nonexistent_key", value));
}

BOOST_AUTO_TEST_CASE(test_connection_retry) {
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }
    // Test with invalid host to trigger retry mechanism
    RedisCache cache("invalid_host", port_, 100, 2, 100);

    // Operations should fail after retries
    BOOST_CHECK(!cache.Set("test_key", "test_value"));

    std::string value;
    BOOST_CHECK(!cache.Get("test_key", value));
}

BOOST_AUTO_TEST_CASE(test_concurrent_operations) {
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }
    RedisCache cache(host_, port_, 1000, retry_count_, retry_delay_ms_);

    // Set multiple keys
    BOOST_CHECK(cache.Set("key1", "value1"));
    BOOST_CHECK(cache.Set("key2", "value2"));

    // Get multiple keys
    std::string value1, value2;
    BOOST_CHECK(cache.Get("key1", value1));
    BOOST_CHECK(cache.Get("key2", value2));

    BOOST_CHECK_EQUAL(value1, "value1");
    BOOST_CHECK_EQUAL(value2, "value2");

    // Clear should remove all keys
    BOOST_CHECK(cache.Clear());
    BOOST_CHECK(!cache.Get("key1", value1));
    BOOST_CHECK(!cache.Get("key2", value2));
}

BOOST_AUTO_TEST_CASE(test_empty_values) {
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }
    RedisCache cache(host_, port_, 1000, retry_count_, retry_delay_ms_);

    // Test empty string value
    BOOST_CHECK(cache.Set("empty_key", ""));
    std::string value;
    BOOST_CHECK(cache.Get("empty_key", value));
    BOOST_CHECK_EQUAL(value, "");
}

BOOST_AUTO_TEST_CASE(test_redis_server_not_configured) {
    // Test behavior when Redis host is not configured in environment
    const char* original_host = std::getenv("OPENMIND_TEST_REDIS_HOST");
    unsetenv("OPENMIND_TEST_REDIS_HOST");

    RedisCache cache("localhost", 6379, 100, 1, 100);
    std::string value;
    BOOST_CHECK(!cache.Set("test_key", "test_value"));
    BOOST_CHECK(!cache.Get("test_key", value));

    if (original_host) {
        setenv("OPENMIND_TEST_REDIS_HOST", original_host, 1);
    }
}

BOOST_AUTO_TEST_CASE(test_redis_version_requirement) {
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }

    RedisCache cache(host_, port_, 1000, retry_count_, retry_delay_ms_);
    std::string version = cache.GetVersion();
    BOOST_TEST_MESSAGE("Redis version: " + version);

    // Check both library and server versions
    std::string hiredisVer = HIREDIS_VERSION;
    BOOST_TEST_MESSAGE("hiredis version: " + hiredisVer);

    std::string error_msg;
    bool is_compatible = RedisCache::IsVersionCompatible(version, &error_msg);
    BOOST_TEST_MESSAGE("Minimum required version: " + RedisCache::GetMinimumRequiredVersion());
    BOOST_CHECK_MESSAGE(
        is_compatible,
        error_msg.empty() ? "Version check failed" : error_msg
    );
    BOOST_CHECK(version != "0.0.0"); // Ensure we got a valid version
    BOOST_CHECK(!version.empty());    // Double check version string validity
}

BOOST_AUTO_TEST_CASE(test_redis_server_not_running, *boost::unit_test::disabled()) {
    // This test is disabled by default since it requires Redis to be NOT running
    // Enable this test only when specifically testing behavior without Redis server
    BOOST_CHECK_THROW({
        RedisCache cache(host_, port_, 100, 1, 100);
    }, std::runtime_error);
}

BOOST_AUTO_TEST_CASE(test_redis_connection_timeout) {
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }

    // Test with a very short timeout
    RedisCache cache(host_, port_, 1, retry_count_, retry_delay_ms_);
    std::string value;

    // Operations should fail due to timeout
    BOOST_CHECK(!cache.Set("timeout_key", "timeout_value"));
    BOOST_CHECK(!cache.Get("timeout_key", value));
}

BOOST_AUTO_TEST_CASE(test_error_messages) {
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }

    // Test with invalid host to check error messages
    RedisCache cache("invalid_host", port_, 100, 1, 100);

    // Check error message
    std::string error = cache.GetLastError();
    BOOST_TEST_MESSAGE("Error message: " + error);
    BOOST_CHECK(!error.empty());

    // Check hiredis version
    std::string hiredisVersion = RedisCache::GetHiredisVersion();
    BOOST_TEST_MESSAGE("Hiredis version: " + hiredisVersion);
    BOOST_CHECK(!hiredisVersion.empty());

    // Version should be default for invalid server
    BOOST_CHECK_EQUAL(cache.GetVersion(), "0.0.0");
}

BOOST_AUTO_TEST_CASE(test_version_check_with_invalid_server) {
    // Test version check behavior with invalid server
    RedisCache cache("invalid_host", 6379, 100, 2, 100);
    std::string version = cache.GetVersion();
    BOOST_CHECK_EQUAL(version, "0.0.0");  // Should return default version for invalid server
    BOOST_CHECK(!RedisCache::IsVersionCompatible(version));
}

BOOST_AUTO_TEST_CASE(test_version_check_with_retries) {
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }

    // Create cache with retry settings
    RedisCache cache(host_, port_, 1000, 3, 100);

    // Get version and verify compatibility multiple times to test retry mechanism
    for (int i = 0; i < 3; ++i) {
        std::string version = cache.GetVersion();
        BOOST_TEST_MESSAGE("Attempt " + std::to_string(i + 1) + " - Redis version: " + version);
        BOOST_CHECK_MESSAGE(
            RedisCache::IsVersionCompatible(version),
            "Redis version check failed on attempt " << (i + 1)
        );
    }
}

BOOST_AUTO_TEST_CASE(test_version_parsing_edge_cases) {
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }

    RedisCache cache(host_, port_, 1000, retry_count_, retry_delay_ms_);

    // Test malformed version strings
    BOOST_CHECK(!RedisCache::IsVersionCompatible("not.a.version"));
    BOOST_CHECK(!RedisCache::IsVersionCompatible("1.a.0"));
    BOOST_CHECK(!RedisCache::IsVersionCompatible(""));

    // Test version comparison edge cases
    BOOST_CHECK(RedisCache::IsVersionCompatible("999.999.999")); // Future version
    BOOST_CHECK(!RedisCache::IsVersionCompatible("0.0.1")); // Too old
}

BOOST_AUTO_TEST_CASE(test_connection_pool_exhaustion) {
    if (!std::getenv("OPENMIND_TEST_REDIS_HOST")) {
        BOOST_TEST_MESSAGE("Skipping Redis tests - Redis host not configured");
        return;
    }

    RedisCache cache(host_, port_, 100, 3, 50);
    std::vector<std::thread> threads;
    std::atomic<int> failures(0);
    std::atomic<int> retries(0);
    std::atomic<int> timeouts(0);

    BOOST_TEST_MESSAGE("Starting connection pool exhaustion test:");
    BOOST_TEST_MESSAGE("  Threads: 100");
    BOOST_TEST_MESSAGE("  Timeout: 100ms");
    BOOST_TEST_MESSAGE("  Retry Count: 3");
    BOOST_TEST_MESSAGE("  Retry Delay: 50ms");

    // Spawn multiple threads to attempt concurrent connections
    for (int i = 0; i < 100; ++i) {
        threads.emplace_back([&cache, &failures, &retries, &timeouts, i]() {
            std::string test_key = "key_" + std::to_string(i);
            if (!cache.Set(test_key, "test_value")) {
                failures++;
                std::string error = cache.GetLastError();
                if (error.find("timeout") != std::string::npos) {
                    timeouts++;
                }
                if (error.find("retry") != std::string::npos) {
                    retries++;
                }
            }
        });
    }

    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }

    // Report detailed statistics
    BOOST_TEST_MESSAGE("Connection Pool Test Results:");
    BOOST_TEST_MESSAGE("  Total Failures: " + std::to_string(failures));
    BOOST_TEST_MESSAGE("  Timeout Errors: " + std::to_string(timeouts));
    BOOST_TEST_MESSAGE("  Retry Attempts: " + std::to_string(retries));

    // We expect some operations to fail due to connection pool exhaustion
    BOOST_CHECK_MESSAGE(failures > 0,
        "Expected some operations to fail due to connection pool exhaustion\n"
        "Failures: " + std::to_string(failures) + "\n"
        "Timeouts: " + std::to_string(timeouts) + "\n"
        "Retries: " + std::to_string(retries));
}

BOOST_AUTO_TEST_SUITE_END()
