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

# Add tests subdirectory
add_subdirectory(tests)
