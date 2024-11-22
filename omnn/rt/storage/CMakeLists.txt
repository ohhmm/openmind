cmake_minimum_required(VERSION 3.15)

# Find required packages
find_package(hiredis REQUIRED)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# Add Redis cache library
add_library(redis_cache
    RedisCache.cpp
    RedisCache.h
)

target_link_libraries(redis_cache
    PUBLIC
    hiredis::hiredis
    cache_base
)

target_include_directories(redis_cache
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Configure Redis connection settings from environment
target_compile_definitions(redis_cache
    PRIVATE
    REDIS_HOST="$ENV{REDIS_HOST}"
    REDIS_PORT=$ENV{REDIS_PORT}
)

# Add tests subdirectory
add_subdirectory(tests)
