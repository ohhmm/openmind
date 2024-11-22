# Storage tests configuration
cmake_minimum_required(VERSION 3.15)

# Set CMake policies
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.27)
    cmake_policy(SET CMP0167 NEW)  # Use modern Boost finding
endif()
cmake_policy(SET CMP0002 NEW)  # Target names must be unique

set(DEPENDENCIES
    storage
)

# Use Conan-provided Boost targets
find_package(Boost CONFIG REQUIRED)
list(APPEND DEPENDENCIES Boost::headers)

# Redis cache test configuration
if(OPENMIND_STORAGE_REDIS)
    # Configure Redis cache test
    add_executable(rt_storage_redis_cache_test redis_cache_test.cpp)
    target_link_libraries(rt_storage_redis_cache_test PRIVATE ${DEPENDENCIES})

    # Set default values for Redis configuration
    set(REDIS_HOST "127.0.0.1")
    set(REDIS_PORT "6379")
    set(REDIS_RETRY_COUNT "10")
    set(REDIS_RETRY_DELAY "2000")
    set(REDIS_LOG_LEVEL "INFO")
    set(TEST_TIMEOUT 120)

    # Platform-specific configurations
    if(WIN32 AND OPENMIND_STORAGE_REDIS_MEMURAI)
        set(TEST_TIMEOUT 300)  # 5 minutes timeout for Windows
        set(REDIS_RETRY_COUNT "15")
        set(REDIS_RETRY_DELAY "5000")
        set(REDIS_LOG_LEVEL "DEBUG")
        set(REDIS_EXECUTABLE "C:/Program Files/Memurai Developer/memurai-developer.exe")

        if(NOT EXISTS "${REDIS_EXECUTABLE}")
            message(WARNING "Memurai executable not found at ${REDIS_EXECUTABLE}")
        endif()
    endif()

    add_test(NAME rt_storage_redis_cache_test COMMAND rt_storage_redis_cache_test)
    set_tests_properties(rt_storage_redis_cache_test PROPERTIES
        TIMEOUT ${TEST_TIMEOUT}
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "Tests/Storage"
        ENVIRONMENT "OPENMIND_TEST_REDIS_HOST=${REDIS_HOST};OPENMIND_TEST_REDIS_PORT=${REDIS_PORT};OPENMIND_TEST_REDIS_RETRY_COUNT=${REDIS_RETRY_COUNT};OPENMIND_TEST_REDIS_RETRY_DELAY=${REDIS_RETRY_DELAY};OPENMIND_TEST_REDIS_LOG_LEVEL=${REDIS_LOG_LEVEL}"
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
