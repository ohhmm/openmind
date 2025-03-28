cmake_minimum_required(VERSION 3.15)

# Find required packages
find_package(Boost REQUIRED COMPONENTS unit_test_framework system thread filesystem)
find_package(Threads REQUIRED)

# Add Redis cache library
add_library(redis_cache
    RedisCache.cpp
    RedisCache.h
)

# Find cache_base target
if(TARGET cache_base)
    set(CACHE_BASE_LIB cache_base)
else()
    # Fallback to direct path if target not available
    set(CACHE_BASE_LIB ${CMAKE_BINARY_DIR}/omnn/storage/Debug/libcache_base.a)
endif()

target_link_libraries(redis_cache
    PUBLIC
    Boost::system
    Boost::thread
    Boost::filesystem
    Threads::Threads
    ${CACHE_BASE_LIB}
    ${CMAKE_DL_LIBS}
    -lboost_filesystem
    -lboost_system
)

target_include_directories(redis_cache
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../storage
)

# Add tests subdirectory
add_subdirectory(tests)
