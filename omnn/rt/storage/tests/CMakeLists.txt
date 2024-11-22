# Storage tests configuration
set(DEPENDENCIES
    storage
)

# Find Boost components for testing
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# Set common test properties
set(BOOST_TEST_LIBS Boost::unit_test_framework)

# Redis cache test configuration
if(OPENMIND_STORAGE_REDIS)
    list(APPEND DEPENDENCIES
        hiredis::hiredis
        ${BOOST_TEST_LIBS}
    )

    # Configure Redis cache test
    add_executable(redis_cache_test redis_cache_test.cpp)
    target_link_libraries(redis_cache_test PRIVATE ${DEPENDENCIES})
    target_compile_definitions(redis_cache_test PRIVATE
        BOOST_TEST_DYN_LINK
        BOOST_TEST_MODULE=redis_cache_test
    )

    # Set test properties with increased timeout for Windows
    if(WIN32)
        set(TEST_TIMEOUT 300)  # 5 minutes timeout for Windows
        if(OPENMIND_STORAGE_REDIS_MEMURAI)
            set(REDIS_HOST "localhost")
            set(REDIS_PORT "6379")
            set(REDIS_EXECUTABLE "C:/Program Files/Memurai/memurai-developer.exe")
        endif()
    else()
        set(TEST_TIMEOUT 120)  # 2 minutes timeout for other platforms
    endif()

    add_test(NAME redis_cache_test COMMAND redis_cache_test)
    set_tests_properties(redis_cache_test PROPERTIES
        TIMEOUT ${TEST_TIMEOUT}
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "Tests/Storage"
        ENVIRONMENT "OPENMIND_TEST_REDIS_RETRY_COUNT=10;OPENMIND_TEST_REDIS_RETRY_DELAY=3000;OPENMIND_TEST_REDIS_HOST=${REDIS_HOST};OPENMIND_TEST_REDIS_PORT=${REDIS_PORT}"
    )
endif()

# LevelDB cache test configuration
if(OPENMIND_STORAGE_LEVELDB)
    list(APPEND DEPENDENCIES leveldb ${BOOST_TEST_LIBS})
    add_executable(leveldb_cache_test leveldb_cache_test.cpp)
    target_link_libraries(leveldb_cache_test PRIVATE ${DEPENDENCIES})
    target_compile_definitions(leveldb_cache_test PRIVATE
        BOOST_TEST_DYN_LINK
        BOOST_TEST_MODULE=leveldb_cache_test
    )
    add_test(NAME leveldb_cache_test COMMAND leveldb_cache_test)
    set_target_properties(leveldb_cache_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "Tests/Storage"
    )
endif()
