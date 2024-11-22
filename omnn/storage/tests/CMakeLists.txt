include(CTest)

if(OPENMIND_STORAGE_REDIS)
    add_executable(redis_cache_test redis_cache_test.cpp)
    target_link_libraries(redis_cache_test PRIVATE
        storage
        rt
        leveldb
        pthread
        hiredis
        ${BOOST_TEST_LINK_LIBS}  # This includes both unit_test_framework and test_exec_monitor
        ${BOOST_LINK_LIBS}       # Add core Boost libraries
    )
    add_test(NAME redis_cache_test COMMAND redis_cache_test)
endif()
