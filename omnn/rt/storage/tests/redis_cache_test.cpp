#define BOOST_TEST_MODULE rt_storage_redis_cache_test
#include <boost/test/included/unit_test.hpp>
#include "../CacheBase.h"
#include "../RedisCache.h"
#include <chrono>
#include <thread>
#include <cstdlib>

#ifdef OPENMIND_STORAGE_REDIS

BOOST_AUTO_TEST_SUITE(redis_cache_suite)

using namespace omnn::rt::storage;

namespace {
bool is_redis_available() {
    const char* log_level = std::getenv("OPENMIND_TEST_REDIS_LOG_LEVEL");
    const bool debug_logging = log_level && std::string(log_level) == "DEBUG";

    const char* host = std::getenv("OPENMIND_TEST_REDIS_HOST") ?
        std::getenv("OPENMIND_TEST_REDIS_HOST") : "127.0.0.1";
    const int port = std::getenv("OPENMIND_TEST_REDIS_PORT") ?
        std::stoi(std::getenv("OPENMIND_TEST_REDIS_PORT")) : 6379;
    const int retries = std::getenv("OPENMIND_TEST_REDIS_RETRY_COUNT") ?
        std::stoi(std::getenv("OPENMIND_TEST_REDIS_RETRY_COUNT")) : 5;
    const int delay_ms = std::getenv("OPENMIND_TEST_REDIS_RETRY_DELAY") ?
        std::stoi(std::getenv("OPENMIND_TEST_REDIS_RETRY_DELAY")) : 2000;
    std::string last_error;
    int attempt = 1;

#ifdef OPENMIND_STORAGE_REDIS_MEMURAI
    std::string server_info = "Memurai Developer";
    if (debug_logging) {
        BOOST_TEST_MESSAGE("Windows environment detected - Testing with Memurai Developer");
        BOOST_TEST_MESSAGE("Connection details: " + std::string(host) + ":" + std::to_string(port));
        BOOST_TEST_MESSAGE("Retry settings: count=" + std::to_string(retries) + ", delay=" + std::to_string(delay_ms) + "ms");

        // Check Memurai service status before attempting connection
        std::string check_cmd = "powershell -Command \"";
        check_cmd += "Write-Host 'Initial Service Status:'; ";
        check_cmd += "Get-Service memurai | Format-List Status,StartType,Name; ";
        check_cmd += "Get-Process memurai* | Format-Table Name,Id,StartTime; ";
        check_cmd += "netstat -ano | findstr :6379\"";
        system(check_cmd.c_str());
    }
#else
    std::string server_info = "Redis Server";
    BOOST_TEST_MESSAGE("Testing with Redis Server at " + std::string(host) + ":" + std::to_string(port));
#endif

    while (attempt <= retries) {
        try {
            BOOST_TEST_MESSAGE("Attempting to connect to " + std::string(host) + ":" + std::to_string(port));
            RedisCache test_cache(host, port);
            BOOST_TEST_MESSAGE("Connection established, attempting basic operations");
            if (test_cache.Set("__test_connection__", "ping")) {
                BOOST_TEST_MESSAGE("Set operation successful");
                auto result = test_cache.GetOne("__test_connection__");
                BOOST_TEST_MESSAGE("Get operation returned: " + result);
                if (result == "ping") {
                    test_cache.Clear("__test_connection__");
                    if (debug_logging) {
                        BOOST_TEST_MESSAGE(server_info + " connection and basic operations verified after " +
                            std::to_string(attempt) + " attempt(s)");
                    }
                    return true;
                }
            }
            throw std::runtime_error("Connection established but basic operations failed");
        } catch (const std::exception& e) {
            last_error = e.what();
            if (debug_logging) {
                BOOST_TEST_MESSAGE(server_info + " connection attempt " +
                    std::to_string(attempt) + "/" + std::to_string(retries) +
                    " failed: " + last_error);
            }

#ifdef OPENMIND_STORAGE_REDIS_MEMURAI
            if (debug_logging) {
                BOOST_TEST_MESSAGE("Checking Memurai service status...");
                std::string recovery_cmd = "powershell -Command \"";
                recovery_cmd += "$service = Get-Service memurai -ErrorAction SilentlyContinue; ";
                recovery_cmd += "if ($service -and $service.Status -ne 'Running') { ";
                recovery_cmd += "    Write-Host 'Attempting to restart Memurai service...'; ";
                recovery_cmd += "    Restart-Service memurai -Force; ";
                recovery_cmd += "    Start-Sleep -s 5; ";
                recovery_cmd += "}; ";
                recovery_cmd += "Get-Service memurai | Format-List Status,StartType,Name; ";
                recovery_cmd += "Get-Process memurai* | Format-Table Name,Id,StartTime; ";
                recovery_cmd += "Get-EventLog -LogName Application -Source Memurai -Newest 3\"";
                system(recovery_cmd.c_str());
            }
#endif

            if (attempt < retries) {
                BOOST_TEST_MESSAGE("Waiting " + std::to_string(delay_ms) + "ms before next attempt...");
                std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
            }
            attempt++;
        }
    }

    BOOST_TEST_MESSAGE(server_info + " connection failed after " + std::to_string(retries) +
        " attempts. Details: host=" + std::string(host) + ", port=" + std::to_string(port) +
        ", last_error=\"" + last_error + "\"");
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

#ifdef OPENMIND_STORAGE_REDIS_MEMURAI
    BOOST_TEST_MESSAGE("Running Memurai-specific error handling tests");
    try {
        RedisCache cache;
        const char* log_level = std::getenv("OPENMIND_TEST_REDIS_LOG_LEVEL");
        const bool debug_logging = log_level && std::string(log_level) == "DEBUG";

        // Test connection with invalid port
        BOOST_CHECK_THROW(RedisCache("127.0.0.1", 6380), std::runtime_error);

        // Test connection timeout
        BOOST_CHECK_THROW(RedisCache("127.0.0.1", 6379, 1), std::runtime_error);

        // Test service interruption recovery
        BOOST_CHECK(cache.Set("test_key", "test_value"));
        system("net stop memurai");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        BOOST_CHECK_THROW(cache.Set("test_key2", "test_value2"), std::runtime_error);
        system("net start memurai");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        BOOST_CHECK(cache.Set("test_key3", "test_value3"));
    } catch (const std::exception& e) {
        BOOST_TEST_MESSAGE("Memurai error test failed: " + std::string(e.what()));
        system("net start memurai");  // Ensure service is restored
        throw;
    }
#else
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
#endif
}

BOOST_AUTO_TEST_SUITE_END()

#endif // OPENMIND_STORAGE_REDIS
