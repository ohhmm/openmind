# Storage tests configuration
set(DEPENDENCIES
    storage
)

# Redis cache test configuration
if(OPENMIND_STORAGE_REDIS)
    list(APPEND DEPENDENCIES hiredis::hiredis)

    # Configure Redis cache test
    add_executable(redis_cache_test redis_cache_test.cpp)
    target_link_libraries(redis_cache_test PRIVATE ${DEPENDENCIES} ${BOOST_TEST_LINK_LIBS})

    # Set test properties with increased timeout for Windows
    if(WIN32)
        set(TEST_TIMEOUT 300)  # 5 minutes timeout for Windows
    else()
        set(TEST_TIMEOUT 120)  # 2 minutes timeout for other platforms
    endif()

    add_test(NAME redis_cache_test COMMAND redis_cache_test)
    set_tests_properties(redis_cache_test PROPERTIES
        TIMEOUT ${TEST_TIMEOUT}
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "Tests/Storage"
        ENVIRONMENT "OPENMIND_TEST_REDIS_RETRY_COUNT=5;OPENMIND_TEST_REDIS_RETRY_DELAY=3000"
    )
endif()

# LevelDB cache test configuration
if(OPENMIND_STORAGE_LEVELDB)
    list(APPEND DEPENDENCIES leveldb)
    add_executable(leveldb_cache_test leveldb_cache_test.cpp)
    target_link_libraries(leveldb_cache_test PRIVATE ${DEPENDENCIES} ${BOOST_TEST_LINK_LIBS})
    add_test(NAME leveldb_cache_test COMMAND leveldb_cache_test)
    set_target_properties(leveldb_cache_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "Tests/Storage"
    )
endif()
