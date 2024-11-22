cmake_minimum_required(VERSION 3.15)

# Redis cache test
add_executable(redis_cache_test redis_cache_test.cpp)

target_link_libraries(redis_cache_test
    PRIVATE
    redis_cache
    Boost::unit_test_framework
)

add_test(NAME redis_cache_test COMMAND redis_cache_test)

# Set test properties
set_tests_properties(redis_cache_test PROPERTIES
    ENVIRONMENT "REDIS_HOST=$ENV{REDIS_HOST};REDIS_PORT=$ENV{REDIS_PORT};OPENMIND_TEST_REDIS_RETRY_COUNT=5;OPENMIND_TEST_REDIS_RETRY_DELAY=1000"
    TIMEOUT 60
    FIXTURES_REQUIRED "REDIS_SERVICE"
)
