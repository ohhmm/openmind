#define BOOST_TEST_MODULE redis_cache_test
#include <boost/test/unit_test.hpp>
#include "../RedisCache.h"
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

using namespace omnn::rt::storage;

struct RedisTestConfig {
    RedisTestConfig() {
        // Get configuration from environment
        const char* redis_host = std::getenv("REDIS_HOST");
        const char* redis_port = std::getenv("REDIS_PORT");
        const char* retry_count = std::getenv("OPENMIND_TEST_REDIS_RETRY_COUNT");
        const char* retry_delay = std::getenv("OPENMIND_TEST_REDIS_RETRY_DELAY");

        host_ = redis_host ? redis_host : "";
        port_ = redis_port ? std::stoi(redis_port) : 0;
        retry_count_ = retry_count ? std::stoi(retry_count) : 5;
        retry_delay_ms_ = retry_delay ? std::stoi(retry_delay) : 1000;
    }

    std::string host_;
    int port_;
    int retry_count_;
    int retry_delay_ms_;
};

BOOST_FIXTURE_TEST_SUITE(redis_cache_tests, RedisTestConfig)

BOOST_AUTO_TEST_CASE(test_basic_operations) {
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
    RedisCache cache(host_, port_, 1000, retry_count_, retry_delay_ms_);

    std::string value;
    BOOST_CHECK(!cache.Get("nonexistent_key", value));
}

BOOST_AUTO_TEST_CASE(test_connection_retry) {
    // Test with invalid host to trigger retry mechanism
    RedisCache cache("invalid_host", 6379, 100, 2, 100);

    // Operations should fail after retries
    BOOST_CHECK(!cache.Set("test_key", "test_value"));

    std::string value;
    BOOST_CHECK(!cache.Get("test_key", value));
}

BOOST_AUTO_TEST_CASE(test_connection_timeout) {
    // Test with very short timeout
    RedisCache cache(host_, port_, 1, retry_count_, 100);

    // Operations should still succeed due to retry mechanism
    BOOST_CHECK(cache.Set("timeout_test", "test_value"));

    std::string value;
    BOOST_CHECK(cache.Get("timeout_test", value));
    BOOST_CHECK_EQUAL(value, "test_value");
}

BOOST_AUTO_TEST_CASE(test_binary_data_error_cases) {
    RedisCache cache(host_, port_, 1000, retry_count_, retry_delay_ms_);

    // Test with null data
    BOOST_CHECK(!cache.SetBinary("null_key", nullptr, 0));

    // Test with invalid key
    std::vector<char> data = {0x00, 0x01};
    std::vector<char> retrieved_data;
    BOOST_CHECK(!cache.GetBinary("nonexistent_key", retrieved_data));
    BOOST_CHECK(retrieved_data.empty());
}

BOOST_AUTO_TEST_CASE(test_binary_data) {
    RedisCache cache(host_, port_, 1000, retry_count_, retry_delay_ms_);

    // Test binary data storage
    const std::vector<char> test_data = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0xFD};
    BOOST_CHECK(cache.SetBinary("binary_key", test_data.data(), test_data.size()));

    std::vector<char> retrieved_data;
    BOOST_CHECK(cache.GetBinary("binary_key", retrieved_data));
    BOOST_CHECK_EQUAL_COLLECTIONS(test_data.begin(), test_data.end(),
                                retrieved_data.begin(), retrieved_data.end());
}

BOOST_AUTO_TEST_CASE(test_default_connection_params) {
    // Test with empty host and port to trigger default values
    RedisCache cache;

    // Operations should succeed with default connection params
    BOOST_CHECK(cache.Set("default_test_key", "default_test_value"));

    std::string value;
    BOOST_CHECK(cache.Get("default_test_key", value));
    BOOST_CHECK_EQUAL(value, "default_test_value");
}

BOOST_AUTO_TEST_CASE(test_service_unavailable) {
    // Test behavior when Redis service is unavailable
    RedisCache cache("localhost", 6380);  // Use incorrect port

    // Operations should fail after retries
    BOOST_CHECK(!cache.Set("unavailable_test", "test_value"));

    std::string value;
    BOOST_CHECK(!cache.Get("unavailable_test", value));
    BOOST_CHECK(value.empty());
}

BOOST_AUTO_TEST_CASE(test_binary_memory_management) {
    RedisCache cache(host_, port_);

    // Create large binary data
    std::vector<char> large_data(1024 * 1024);
    for (size_t i = 0; i < large_data.size(); ++i) {
        large_data[i] = static_cast<char>(i % 256);
    }

    // Test multiple set/get cycles to check for memory leaks
    for (int i = 0; i < 10; ++i) {
        BOOST_CHECK(cache.SetBinary("binary_mem_test", large_data.data(), large_data.size()));
        std::vector<char> retrieved_data;
        BOOST_CHECK(cache.GetBinary("binary_mem_test", retrieved_data));
        BOOST_CHECK_EQUAL_COLLECTIONS(large_data.begin(), large_data.end(),
                                    retrieved_data.begin(), retrieved_data.end());
    }
}

BOOST_AUTO_TEST_CASE(test_cleanup) {
    // Test proper cleanup of resources
    {
        RedisCache cache(host_, port_);
        BOOST_CHECK(cache.Set("cleanup_test", "test_value"));
    }  // Destructor should be called here

    // Verify we can create a new connection
    RedisCache new_cache(host_, port_);
    BOOST_CHECK(new_cache.IsConnected());
}

BOOST_AUTO_TEST_CASE(test_connection_recovery) {
    RedisCache cache(host_, port_);
    BOOST_CHECK(cache.Set("recovery_test", "test_value"));

    // Simulate Redis service restart by forcing a connection error
    cache.Disconnect();

    // Operation should succeed due to retry mechanism
    BOOST_CHECK(cache.Set("after_recovery", "new_value"));
    std::string value;
    BOOST_CHECK(cache.Get("after_recovery", value));
    BOOST_CHECK_EQUAL(value, "new_value");
}

BOOST_AUTO_TEST_CASE(test_error_response) {
    RedisCache cache(host_, port_);

    // Try to execute an invalid command to trigger error response
    redisCommand(cache.GetContext(), "INVALID_COMMAND");

    // Next operation should still succeed due to retry mechanism
    BOOST_CHECK(cache.Set("error_test", "test_value"));
    std::string value;
    BOOST_CHECK(cache.Get("error_test", value));
    BOOST_CHECK_EQUAL(value, "test_value");
}

BOOST_AUTO_TEST_CASE(test_large_data) {
    RedisCache cache(host_, port_);

    // Create a large string (1MB)
    std::string large_value(1024 * 1024, 'X');
    BOOST_CHECK(cache.Set("large_key", large_value));

    std::string retrieved_value;
    BOOST_CHECK(cache.Get("large_key", retrieved_value));
    BOOST_CHECK_EQUAL(retrieved_value.size(), large_value.size());
    BOOST_CHECK_EQUAL(retrieved_value, large_value);
}

BOOST_AUTO_TEST_CASE(test_concurrent_access) {
    RedisCache cache(host_, port_);
    const int num_operations = 100;

    // Perform multiple operations concurrently
    std::vector<std::thread> threads;
    for (int i = 0; i < num_operations; ++i) {
        threads.emplace_back([&cache, i]() {
            std::string key = "concurrent_key_" + std::to_string(i);
            std::string value = "value_" + std::to_string(i);
            BOOST_CHECK(cache.Set(key, value));

            std::string retrieved_value;
            BOOST_CHECK(cache.Get(key, retrieved_value));
            BOOST_CHECK_EQUAL(retrieved_value, value);
        });
    }

    // Wait for all operations to complete
    for (auto& thread : threads) {
        thread.join();
    }
}

BOOST_AUTO_TEST_CASE(test_key_expiration) {
    RedisCache cache(host_, port_);

    // Set a key with 1 second expiration
    BOOST_CHECK(cache.Set("expire_test", "test_value", 1));

    // Verify key exists
    std::string value;
    BOOST_CHECK(cache.Get("expire_test", value));
    BOOST_CHECK_EQUAL(value, "test_value");

    // Wait for expiration
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Verify key has expired
    BOOST_CHECK(!cache.Get("expire_test", value));
}

BOOST_AUTO_TEST_CASE(test_binary_memory_management) {
    RedisCache cache(host_, port_);

    // Create large binary data
    std::vector<char> large_data(1024 * 1024);
    for (size_t i = 0; i < large_data.size(); ++i) {
        large_data[i] = static_cast<char>(i % 256);
    }

    // Test multiple set/get cycles to check for memory leaks
    for (int i = 0; i < 10; ++i) {
        BOOST_CHECK(cache.SetBinary("binary_mem_test", large_data.data(), large_data.size()));
        std::vector<char> retrieved_data;
        BOOST_CHECK(cache.GetBinary("binary_mem_test", retrieved_data));
        BOOST_CHECK_EQUAL_COLLECTIONS(large_data.begin(), large_data.end(),
                                    retrieved_data.begin(), retrieved_data.end());
    }
}

BOOST_AUTO_TEST_CASE(test_pipeline_operations) {
    RedisCache cache(host_, port_);

    // Prepare multiple operations
    std::vector<std::pair<std::string, std::string>> operations;
    const int num_operations = 1000;
    for (int i = 0; i < num_operations; ++i) {
        operations.emplace_back(
            "pipeline_key_" + std::to_string(i),
            "value_" + std::to_string(i)
        );
    }

    // Execute pipeline
    BOOST_CHECK(cache.Pipeline(operations));

    // Verify all operations succeeded
    for (const auto& op : operations) {
        std::string value;
        BOOST_CHECK(cache.Get(op.first, value));
        BOOST_CHECK_EQUAL(value, op.second);
    }
}

BOOST_AUTO_TEST_CASE(test_pipeline_with_expire) {
    RedisCache cache(host_, port_);

    // Prepare multiple operations
    std::vector<std::pair<std::string, std::string>> operations;
    const int num_operations = 100;
    for (int i = 0; i < num_operations; ++i) {
        operations.emplace_back(
            "expire_pipeline_key_" + std::to_string(i),
            "value_" + std::to_string(i)
        );
    }

    // Execute pipeline with 1 second expiration
    BOOST_CHECK(cache.PipelineWithExpire(operations, 1));

    // Verify all operations succeeded
    for (const auto& op : operations) {
        std::string value;
        BOOST_CHECK(cache.Get(op.first, value));
        BOOST_CHECK_EQUAL(value, op.second);
    }

    // Wait for expiration
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Verify all keys have expired
    for (const auto& op : operations) {
        std::string value;
        BOOST_CHECK(!cache.Get(op.first, value));
    }
}

BOOST_AUTO_TEST_CASE(test_server_restart_recovery) {
    RedisCache cache(host_, port_, 1000, 10, 500); // 1s timeout, 10 retries, 500ms delay

    // Initial operation
    BOOST_CHECK(cache.Set("restart_test", "initial_value"));

    // Simulate server restart by forcing disconnection
    cache.Disconnect();

    // Operations should succeed after reconnection attempts
    BOOST_CHECK(cache.Set("restart_test", "after_restart"));
    std::string value;
    BOOST_CHECK(cache.Get("restart_test", value));
    BOOST_CHECK_EQUAL(value, "after_restart");
}

BOOST_AUTO_TEST_CASE(test_server_overload_handling) {
    RedisCache cache(host_, port_, 1000, 5, 1000); // 1s timeout, 5 retries, 1s delay

    // Generate load with multiple concurrent operations
    std::vector<std::thread> threads;
    const int num_threads = 50;
    std::atomic<int> success_count{0};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&cache, i, &success_count]() {
            if (cache.Set("overload_key_" + std::to_string(i), "test_value")) {
                success_count++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Some operations should succeed despite overload
    BOOST_CHECK_GT(success_count, 0);
}

BOOST_AUTO_TEST_CASE(test_auth_handling) {
    RedisCache cache(host_, port_);

    // Test basic operations with default (no auth) configuration
    BOOST_CHECK(cache.Set("auth_test", "test_value"));
    std::string value;
    BOOST_CHECK(cache.Get("auth_test", value));
    BOOST_CHECK_EQUAL(value, "test_value");
}

BOOST_AUTO_TEST_CASE(test_connection_pooling) {
    const int pool_size = 5;
    std::vector<std::unique_ptr<RedisCache>> cache_pool;

    // Initialize connection pool
    for (int i = 0; i < pool_size; ++i) {
        cache_pool.emplace_back(std::make_unique<RedisCache>(host_, port_));
    }

    // Test concurrent access using connection pool
    std::vector<std::thread> threads;
    const int operations_per_connection = 1000;
    std::atomic<int> success_count{0};

    for (int i = 0; i < pool_size; ++i) {
        threads.emplace_back([&cache_pool, i, operations_per_connection, &success_count]() {
            auto& cache = cache_pool[i];
            for (int j = 0; j < operations_per_connection; ++j) {
                std::string key = "pool_" + std::to_string(i) + "_key_" + std::to_string(j);
                if (cache->Set(key, "test_value")) {
                    success_count++;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Verify high success rate with connection pooling
    BOOST_CHECK_GT(success_count, pool_size * operations_per_connection * 0.95); // 95% success rate
}

BOOST_AUTO_TEST_CASE(test_cluster_operations) {
    RedisCache cache(host_, port_);

    // Test operations that would typically be distributed across cluster
    const int num_keys = 1000;
    std::atomic<int> success_count{0};
    std::vector<std::thread> threads;

    // Simulate cluster operations with multiple threads
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&cache, i, num_keys, &success_count]() {
            for (int j = 0; j < num_keys / 10; ++j) {
                std::string key = "cluster_" + std::to_string(i) + "_key_" + std::to_string(j);
                std::string value;
                if (cache.Set(key, "test_value") && cache.Get(key, value)) {
                    success_count++;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Verify operations succeeded
    BOOST_CHECK_GT(success_count, num_keys * 0.95); // 95% success rate
}

BOOST_AUTO_TEST_CASE(test_pubsub_operations) {
    RedisCache publisher(host_, port_);
    RedisCache subscriber(host_, port_);

    const std::string channel = "test_channel";
    std::atomic<int> received_count{0};

    // Set up subscription in a separate thread
    std::thread sub_thread([&subscriber, &channel, &received_count]() {
        subscriber.Subscribe(channel, [&received_count](const std::string& msg) {
            if (msg == "test_message") {
                received_count++;
            }
        });
    });

    // Allow time for subscription to be established
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Publish messages
    const int num_messages = 10;
    for (int i = 0; i < num_messages; ++i) {
        BOOST_CHECK(publisher.Publish(channel, "test_message"));
    }

    // Wait for messages to be received
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    subscriber.Unsubscribe(channel);
    sub_thread.join();

    // Verify message reception
    BOOST_CHECK_EQUAL(received_count, num_messages);
}

BOOST_AUTO_TEST_CASE(test_lua_script_execution) {
    RedisCache cache(host_, port_);

    // Test Lua script that increments a value atomically
    const std::string lua_script = R"(
        local key = KEYS[1]
        local value = tonumber(redis.call('get', key) or '0')
        value = value + 1
        redis.call('set', key, value)
        return value
    )";

    // Execute script multiple times concurrently
    std::vector<std::thread> threads;
    const int num_threads = 10;
    std::atomic<bool> all_succeeded{true};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&cache, &lua_script, &all_succeeded]() {
            if (!cache.ExecuteScript(lua_script, {"counter_key"})) {
                all_succeeded = false;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Verify final counter value
    std::string final_value;
    BOOST_CHECK(cache.Get("counter_key", final_value));
    BOOST_CHECK_EQUAL(std::stoi(final_value), num_threads);
    BOOST_CHECK(all_succeeded);
}

BOOST_AUTO_TEST_CASE(test_transaction_operations) {
    RedisCache cache(host_, port_);

    // Test atomic transaction operations
    std::vector<std::pair<std::string, std::string>> operations = {
        {"tx_key1", "value1"},
        {"tx_key2", "value2"},
        {"tx_key3", "value3"}
    };

    // Execute transaction
    BOOST_CHECK(cache.Transaction([&cache, &operations]() {
        bool success = true;
        for (const auto& op : operations) {
            if (!cache.Set(op.first, op.second)) {
                success = false;
                break;
            }
        }
        return success;
    }));

    // Verify all operations succeeded atomically
    for (const auto& op : operations) {
        std::string value;
        BOOST_CHECK(cache.Get(op.first, value));
        BOOST_CHECK_EQUAL(value, op.second);
    }
}

BOOST_AUTO_TEST_CASE(test_scan_operations) {
    RedisCache cache(host_, port_);

    // Insert test data
    const int num_keys = 1000;
    for (int i = 0; i < num_keys; ++i) {
        BOOST_CHECK(cache.Set("scan_key_" + std::to_string(i), "value_" + std::to_string(i)));
    }

    // Test scanning with pattern
    std::vector<std::string> found_keys;
    BOOST_CHECK(cache.Scan("scan_key_*", found_keys));
    BOOST_CHECK_EQUAL(found_keys.size(), num_keys);

    // Verify key contents
    for (const auto& key : found_keys) {
        std::string value;
        BOOST_CHECK(cache.Get(key, value));
        BOOST_CHECK(value.find("value_") != std::string::npos);
    }
}

BOOST_AUTO_TEST_CASE(test_sorted_set_operations) {
    RedisCache cache(host_, port_);

    // Add members to sorted set
    std::vector<std::pair<std::string, double>> members = {
        {"member1", 1.0},
        {"member2", 2.0},
        {"member3", 3.0}
    };

    for (const auto& member : members) {
        BOOST_CHECK(cache.ZAdd("sorted_set", member.first, member.second));
    }

    // Get range by score
    std::vector<std::string> range;
    BOOST_CHECK(cache.ZRangeByScore("sorted_set", 1.0, 2.0, range));
    BOOST_CHECK_EQUAL(range.size(), 2);
    BOOST_CHECK_EQUAL(range[0], "member1");
    BOOST_CHECK_EQUAL(range[1], "member2");
}

BOOST_AUTO_TEST_CASE(test_hash_operations) {
    RedisCache cache(host_, port_);

    // Test hash operations
    std::unordered_map<std::string, std::string> fields = {
        {"field1", "value1"},
        {"field2", "value2"},
        {"field3", "value3"}
    };

    // Set multiple hash fields
    BOOST_CHECK(cache.HMSet("test_hash", fields));

    // Get all fields
    std::unordered_map<std::string, std::string> retrieved_fields;
    BOOST_CHECK(cache.HGetAll("test_hash", retrieved_fields));
    BOOST_CHECK_EQUAL(retrieved_fields.size(), fields.size());

    // Verify field values
    for (const auto& field : fields) {
        BOOST_CHECK_EQUAL(retrieved_fields[field.first], field.second);
    }
}

BOOST_AUTO_TEST_CASE(test_error_handling) {
    RedisCache cache(host_, port_);

    // Test invalid command handling
    std::string result;
    BOOST_CHECK(!cache.ExecuteCommand("INVALID_COMMAND", result));

    // Test connection recovery
    const std::string test_key = "recovery_test";
    const std::string test_value = "test_value";

    // Force connection reset
    cache.ResetConnection();

    // Verify cache operations still work
    BOOST_CHECK(cache.Set(test_key, test_value));
    std::string retrieved_value;
    BOOST_CHECK(cache.Get(test_key, retrieved_value));
    BOOST_CHECK_EQUAL(retrieved_value, test_value);
}

BOOST_AUTO_TEST_CASE(test_connection_pool) {
    const int pool_size = 5;
    RedisCache cache(host_, port_, pool_size);

    // Test parallel operations using connection pool
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};

    for (int i = 0; i < pool_size * 2; ++i) {
        threads.emplace_back([&cache, i, &success_count]() {
            std::string key = "pool_test_" + std::to_string(i);
            std::string value = "value_" + std::to_string(i);
            if (cache.Set(key, value)) {
                std::string retrieved;
                if (cache.Get(key, retrieved) && retrieved == value) {
                    success_count++;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    BOOST_CHECK_EQUAL(success_count, pool_size * 2);
}

BOOST_AUTO_TEST_CASE(test_cluster_operations) {
    RedisCache cache(host_, port_);

    // Test cluster info
    std::string cluster_info;
    BOOST_CHECK(cache.ClusterInfo(cluster_info));

    // Test cluster nodes
    std::vector<std::string> nodes;
    BOOST_CHECK(cache.ClusterNodes(nodes));

    // Test cluster slots
    std::vector<std::tuple<int, int, std::string, int>> slots;
    BOOST_CHECK(cache.ClusterSlots(slots));
}

BOOST_AUTO_TEST_CASE(test_pubsub_operations) {
    RedisCache cache(host_, port_);
    RedisCache subscriber(host_, port_);

    const std::string channel = "test_channel";
    const std::string message = "test_message";
    bool message_received = false;

    // Subscribe to channel
    BOOST_CHECK(subscriber.Subscribe(channel, [&message_received, &message](const std::string& msg) {
        message_received = (msg == message);
    }));

    // Publish message
    BOOST_CHECK(cache.Publish(channel, message));

    // Wait for message processing
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    BOOST_CHECK(message_received);
}

BOOST_AUTO_TEST_CASE(test_stream_operations) {
    RedisCache cache(host_, port_);
    const std::string stream = "test_stream";

    // Add entries to stream
    std::string id1, id2;
    BOOST_CHECK(cache.XAdd(stream, {{"sensor", "1"}, {"temp", "25.5"}}, id1));
    BOOST_CHECK(cache.XAdd(stream, {{"sensor", "1"}, {"temp", "26.0"}}, id2));

    // Read from stream
    std::vector<std::pair<std::string, std::unordered_map<std::string, std::string>>> entries;
    BOOST_CHECK(cache.XRange(stream, "-", "+", 10, entries));
    BOOST_CHECK_EQUAL(entries.size(), 2);

    // Verify entry contents
    BOOST_CHECK_EQUAL(entries[0].second["sensor"], "1");
    BOOST_CHECK_EQUAL(entries[0].second["temp"], "25.5");
}

BOOST_AUTO_TEST_CASE(test_geo_operations) {
    RedisCache cache(host_, port_);

    const std::string key = "locations";
    const std::vector<std::tuple<std::string, double, double>> locations = {
        {"New York", -73.935242, 40.730610},
        {"Los Angeles", -118.243683, 34.052235},
        {"Chicago", -87.629798, 41.878114}
    };

    // Add locations
    for (const auto& [name, lon, lat] : locations) {
        BOOST_CHECK(cache.GeoAdd(key, lon, lat, name));
    }

    // Get distance between two points (in km)
    double distance;
    BOOST_CHECK(cache.GeoDist(key, "New York", "Chicago", distance));
    BOOST_CHECK_CLOSE(distance, 1150.0, 5.0); // Within 5% of actual distance
}

BOOST_AUTO_TEST_CASE(test_bit_operations) {
    RedisCache cache(host_, port_);

    const std::string key = "bitfield_test";
    const std::vector<std::pair<uint32_t, bool>> bits = {
        {0, true}, {1, false}, {2, true}, {100, true}
    };

    // Set bits
    for (const auto& bit : bits) {
        BOOST_CHECK(cache.SetBit(key, bit.first, bit.second));
    }

    // Verify bits
    for (const auto& bit : bits) {
        bool value;
        BOOST_CHECK(cache.GetBit(key, bit.first, value));
        BOOST_CHECK_EQUAL(value, bit.second);
    }
}

BOOST_AUTO_TEST_CASE(test_pipeline_operations) {
    RedisCache cache(host_, port_);

    // Test pipeline operations
    const int batch_size = 5;
    std::vector<std::pair<std::string, std::string>> operations;

    for (int i = 0; i < batch_size; ++i) {
        operations.emplace_back("pipeline_key_" + std::to_string(i),
                              "value_" + std::to_string(i));
    }

    // Execute pipeline batch
    BOOST_CHECK(cache.Pipeline([&cache, &operations]() {
        bool success = true;
        for (const auto& op : operations) {
            if (!cache.Set(op.first, op.second)) {
                success = false;
                break;
            }
        }
        return success;
    }));

    // Verify all operations succeeded
    for (const auto& op : operations) {
        std::string value;
        BOOST_CHECK(cache.Get(op.first, value));
        BOOST_CHECK_EQUAL(value, op.second);
    }
}

BOOST_AUTO_TEST_CASE(test_expiration_operations) {
    RedisCache cache(host_, port_);

    // Test key expiration
    const std::string key = "expiring_key";
    const int ttl_seconds = 2;

    // Set key with expiration
    BOOST_CHECK(cache.SetEx(key, "test_value", ttl_seconds));

    // Verify TTL
    int64_t remaining_ttl;
    BOOST_CHECK(cache.TTL(key, remaining_ttl));
    BOOST_CHECK_GT(remaining_ttl, 0);
    BOOST_CHECK_LE(remaining_ttl, ttl_seconds);

    // Wait for expiration
    std::this_thread::sleep_for(std::chrono::seconds(ttl_seconds + 1));

    // Verify key has expired
    std::string value;
    BOOST_CHECK(!cache.Get(key, value));
}

BOOST_AUTO_TEST_CASE(test_hyperloglog) {
    RedisCache cache(host_, port_);
    const std::string key = "visitors";

    // Add unique elements
    for (int i = 0; i < 1000; ++i) {
        BOOST_CHECK(cache.PfAdd(key, "user:" + std::to_string(i)));
    }

    // Check cardinality
    int64_t count;
    BOOST_CHECK(cache.PfCount(key, count));
    BOOST_CHECK_GE(count, 950);  // Allow for HLL's inherent error rate
    BOOST_CHECK_LE(count, 1050);
}

BOOST_AUTO_TEST_CASE(test_sorted_set) {
    RedisCache cache(host_, port_);
    const std::string key = "leaderboard";

    // Add scores
    BOOST_CHECK(cache.ZAdd(key, {{"player1", 100}, {"player2", 200}, {"player3", 150}}));

    // Get scores in order
    std::vector<std::pair<std::string, double>> scores;
    BOOST_CHECK(cache.ZRangeWithScores(key, 0, -1, scores));
    BOOST_CHECK_EQUAL(scores.size(), 3);
    BOOST_CHECK_EQUAL(scores[0].first, "player1");
    BOOST_CHECK_EQUAL(scores[2].first, "player2");

    // Get rank
    int64_t rank;
    BOOST_CHECK(cache.ZRank(key, "player2", rank));
    BOOST_CHECK_EQUAL(rank, 2);
}

BOOST_AUTO_TEST_CASE(test_scan_operations) {
    RedisCache cache(host_, port_);

    // Populate cache with test data
    for (int i = 0; i < 100; ++i) {
        BOOST_CHECK(cache.Set("scan_key:" + std::to_string(i), "value" + std::to_string(i)));
    }

    // Test SCAN operation
    std::string cursor = "0";
    std::set<std::string> found_keys;
    do {
        std::vector<std::string> keys;
        BOOST_CHECK(cache.Scan(cursor, "scan_key:*", 10, keys));
        found_keys.insert(keys.begin(), keys.end());
    } while (cursor != "0");

    BOOST_CHECK_EQUAL(found_keys.size(), 100);
}

BOOST_AUTO_TEST_CASE(test_lua_script) {
    RedisCache cache(host_, port_);

    // Test Lua script execution
    const std::string script = R"(
        local key = KEYS[1]
        local value = ARGV[1]
        redis.call('SET', key, value)
        return redis.call('GET', key)
    )";

    std::vector<std::string> keys = {"script_key"};
    std::vector<std::string> args = {"script_value"};

    std::string result;
    BOOST_CHECK(cache.EvalScript(script, keys, args, result));
    BOOST_CHECK_EQUAL(result, "script_value");
}

BOOST_AUTO_TEST_CASE(test_watch_operations) {
    RedisCache cache(host_, port_);
    const std::string key = "watch_key";

    // Set initial value
    BOOST_CHECK(cache.Set(key, "initial"));

    // Watch the key for changes
    BOOST_CHECK(cache.Watch(key));

    // Start transaction
    BOOST_CHECK(cache.Multi());
    BOOST_CHECK(cache.Set(key, "modified"));

    // Simulate concurrent modification
    RedisCache other_client(host_, port_);
    BOOST_CHECK(other_client.Set(key, "concurrent"));

    // Transaction should fail due to concurrent modification
    BOOST_CHECK(!cache.Exec());

    // Verify value was changed by concurrent operation
    std::string value;
    BOOST_CHECK(cache.Get(key, value));
    BOOST_CHECK_EQUAL(value, "concurrent");
}

BOOST_AUTO_TEST_CASE(test_transaction_operations) {
    RedisCache cache(host_, port_);

    // Test MULTI/EXEC transaction
    const std::string key1 = "tx_key1";
    const std::string key2 = "tx_key2";

    BOOST_CHECK(cache.Multi());
    BOOST_CHECK(cache.Set(key1, "value1"));
    BOOST_CHECK(cache.Set(key2, "value2"));
    BOOST_CHECK(cache.Exec());

    // Verify transaction results
    std::string value1, value2;
    BOOST_CHECK(cache.Get(key1, value1));
    BOOST_CHECK(cache.Get(key2, value2));
    BOOST_CHECK_EQUAL(value1, "value1");
    BOOST_CHECK_EQUAL(value2, "value2");

    // Test transaction rollback
    BOOST_CHECK(cache.Multi());
    BOOST_CHECK(cache.Set(key1, "new_value1"));
    BOOST_CHECK(cache.Discard());

    // Verify value wasn't changed after rollback
    BOOST_CHECK(cache.Get(key1, value1));
    BOOST_CHECK_EQUAL(value1, "value1");
}

BOOST_AUTO_TEST_CASE(test_set_operations) {
    RedisCache cache(host_, port_);

    // Test set operations
    const std::string set_key = "test_set";
    const std::vector<std::string> members = {"member1", "member2", "member3"};

    // Add members to set
    for (const auto& member : members) {
        BOOST_CHECK(cache.SAdd(set_key, member));
    }

    // Verify set cardinality
    int64_t card;
    BOOST_CHECK(cache.SCard(set_key, card));
    BOOST_CHECK_EQUAL(card, members.size());

    // Check member existence
    bool exists;
    BOOST_CHECK(cache.SIsMember(set_key, "member1", exists));
    BOOST_CHECK(exists);

    // Get all members
    std::vector<std::string> retrieved_members;
    BOOST_CHECK(cache.SMembers(set_key, retrieved_members));
    BOOST_CHECK_EQUAL(retrieved_members.size(), members.size());
}

BOOST_AUTO_TEST_CASE(test_list_operations) {
    RedisCache cache(host_, port_);

    // Test list operations
    const std::vector<std::string> values = {"value1", "value2", "value3"};
    const std::string list_key = "test_list";

    // Push values to list
    for (const auto& value : values) {
        BOOST_CHECK(cache.LPush(list_key, value));
    }

    // Verify list length
    int64_t length;
    BOOST_CHECK(cache.LLen(list_key, length));
    BOOST_CHECK_EQUAL(length, values.size());

    // Get range and verify values
    std::vector<std::string> retrieved_values;
    BOOST_CHECK(cache.LRange(list_key, 0, -1, retrieved_values));
    BOOST_CHECK_EQUAL(retrieved_values.size(), values.size());

    // Values should be in reverse order due to LPush
    for (size_t i = 0; i < values.size(); ++i) {
        BOOST_CHECK_EQUAL(retrieved_values[i], values[values.size() - 1 - i]);
    }
}

BOOST_AUTO_TEST_CASE(test_error_handling) {
    RedisCache cache(host_, port_);

    // Test invalid command format
    BOOST_CHECK(!cache.ExecuteCommand("INVALID"));

    // Test connection recovery
    cache.ForceDisconnect();  // Simulate connection loss
    BOOST_CHECK(cache.Set("recovery_key", "test_value"));  // Should reconnect

    std::string value;
    BOOST_CHECK(cache.Get("recovery_key", value));
    BOOST_CHECK_EQUAL(value, "test_value");

    // Test timeout handling
    BOOST_CHECK(!cache.ExecuteCommandWithTimeout("SLEEP", 5, 1));  // Should timeout after 1s
}

BOOST_AUTO_TEST_CASE(test_stream_operations) {
    RedisCache cache(host_, port_);
    const std::string stream = "test_stream";
    const std::string group = "test_group";

    // Add entries to stream
    std::string id1, id2;
    BOOST_CHECK(cache.XAdd(stream, {{"sensor", "1"}, {"temperature", "25.5"}}, id1));
    BOOST_CHECK(cache.XAdd(stream, {{"sensor", "2"}, {"temperature", "26.0"}}, id2));

    // Create consumer group
    BOOST_CHECK(cache.XGroupCreate(stream, group, "0"));

    // Read from stream as consumer
    std::vector<std::pair<std::string, std::map<std::string, std::string>>> entries;
    BOOST_CHECK(cache.XReadGroup(group, "consumer1", stream, ">", 2, entries));
    BOOST_CHECK_EQUAL(entries.size(), 2);
    BOOST_CHECK_EQUAL(entries[0].second["temperature"], "25.5");
}

BOOST_AUTO_TEST_CASE(test_geo_operations) {
    RedisCache cache(host_, port_);
    const std::string key = "locations";

    // Add some locations
    BOOST_CHECK(cache.GeoAdd(key, {
        {"New York", -74.006, 40.7128},
        {"London", -0.1278, 51.5074},
        {"Tokyo", 139.6917, 35.6895}
    }));

    // Get distance between cities
    double distance;
    BOOST_CHECK(cache.GeoDist(key, "New York", "London", "km", distance));
    BOOST_CHECK_GT(distance, 5500);  // Approximate distance
    BOOST_CHECK_LT(distance, 5600);

    // Get coordinates
    double lon, lat;
    BOOST_CHECK(cache.GeoPos(key, "Tokyo", lon, lat));
    BOOST_CHECK_CLOSE(lon, 139.6917, 0.01);
    BOOST_CHECK_CLOSE(lat, 35.6895, 0.01);
}

BOOST_AUTO_TEST_CASE(test_pubsub_operations) {
    RedisCache publisher(host_, port_);
    RedisCache subscriber(host_, port_);
    const std::string channel = "test_channel";
    std::atomic<bool> message_received{false};
    std::string received_message;

    // Set up subscription handler
    BOOST_CHECK(subscriber.Subscribe(channel, [&](const std::string& msg) {
        received_message = msg;
        message_received = true;
    }));

    // Allow time for subscription to be established
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Publish message
    BOOST_CHECK(publisher.Publish(channel, "test_message"));

    // Wait for message to be received
    int retries = 10;
    while (!message_received && retries-- > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Verify message was received
    BOOST_CHECK(message_received);
    BOOST_CHECK_EQUAL(received_message, "test_message");

    // Unsubscribe and verify
    BOOST_CHECK(subscriber.Unsubscribe(channel));
}

BOOST_AUTO_TEST_SUITE_END()
