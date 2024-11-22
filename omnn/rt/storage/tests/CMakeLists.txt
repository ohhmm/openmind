# Storage tests configuration
cmake_minimum_required(VERSION 3.15)

# Set CMake policies
cmake_policy(SET CMP0167 NEW)  # Use modern Boost finding
cmake_policy(SET CMP0002 NEW)  # Target names must be unique

set(DEPENDENCIES
    storage
)

# Use Conan-provided Boost targets
find_package(Boost CONFIG REQUIRED)
set(BOOST_TEST_LIBS Boost::unit_test_framework)

# Redis cache test configuration
if(OPENMIND_STORAGE_REDIS)
    # Try finding hiredis through various methods
    if(NOT TARGET hiredis::hiredis)
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
        find_package(hiredis MODULE REQUIRED)
    endif()

    list(APPEND DEPENDENCIES
        hiredis::hiredis
        ${BOOST_TEST_LIBS}
    )

    # Configure Redis cache test
    add_executable(rt_storage_redis_cache_test redis_cache_test.cpp)
    target_link_libraries(rt_storage_redis_cache_test PRIVATE ${DEPENDENCIES})
    target_compile_definitions(rt_storage_redis_cache_test PRIVATE
        BOOST_TEST_DYN_LINK
        BOOST_TEST_MODULE=rt_storage_redis_cache_test
    )

    # Set test properties with platform-specific configurations
    if(WIN32)
        set(TEST_TIMEOUT 300)  # 5 minutes timeout for Windows
        if(OPENMIND_STORAGE_REDIS_MEMURAI)
            set(REDIS_HOST "localhost")
            set(REDIS_PORT "6379")
            set(REDIS_EXECUTABLE "C:/Program Files/Memurai Developer/memurai-developer.exe")
        endif()
    else()
        set(TEST_TIMEOUT 120)  # 2 minutes timeout for other platforms
        set(REDIS_HOST "127.0.0.1")  # Use IP address for Unix platforms
        set(REDIS_PORT "6379")
    endif()

    add_test(NAME rt_storage_redis_cache_test COMMAND rt_storage_redis_cache_test)
    set_tests_properties(rt_storage_redis_cache_test PROPERTIES
        TIMEOUT ${TEST_TIMEOUT}
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "Tests/Storage"
        ENVIRONMENT "OPENMIND_TEST_REDIS_RETRY_COUNT=10;OPENMIND_TEST_REDIS_RETRY_DELAY=3000;OPENMIND_TEST_REDIS_HOST=${REDIS_HOST};OPENMIND_TEST_REDIS_PORT=${REDIS_PORT}"
    )
endif()

# LevelDB cache test configuration
if(OPENMIND_STORAGE_LEVELDB)
    list(APPEND DEPENDENCIES leveldb ${BOOST_TEST_LIBS})
    add_executable(rt_storage_leveldb_cache_test leveldb_cache_test.cpp)
    target_link_libraries(rt_storage_leveldb_cache_test PRIVATE ${DEPENDENCIES})
    target_compile_definitions(rt_storage_leveldb_cache_test PRIVATE
        BOOST_TEST_DYN_LINK
        BOOST_TEST_MODULE=rt_storage_leveldb_cache_test
    )
    add_test(NAME rt_storage_leveldb_cache_test COMMAND rt_storage_leveldb_cache_test)
    set_tests_properties(rt_storage_leveldb_cache_test PROPERTIES
        TIMEOUT 120
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "Tests/Storage"
    )
endif()
