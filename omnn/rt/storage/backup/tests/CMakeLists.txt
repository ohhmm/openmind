include(CTest)

if(OPENMIND_STORAGE_REDIS)
    add_executable(redis_cache_test redis_cache_test.cpp)
    target_compile_definitions(redis_cache_test PRIVATE
        BOOST_TEST_DYN_LINK
    )
    target_link_libraries(redis_cache_test PRIVATE
        storage
        rt
        leveldb
        hiredis
        ${BOOST_TEST_LINK_LIBS}  # This includes both unit_test_framework and test_exec_monitor properly configured
    )

    if(NOT WIN32)
        target_link_libraries(redis_cache_test PRIVATE pthread)
    endif()

    add_test(NAME redis_cache_test COMMAND redis_cache_test)
    set_tests_properties(redis_cache_test PROPERTIES
        ENVIRONMENT "REDIS_HOST=localhost;REDIS_PORT=6379"
        TIMEOUT 120  # Increased timeout for CI environments
    )
endif()
