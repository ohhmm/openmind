test()

if(OPENMIND_STORAGE_REDIS)
    add_boost_test(redis_cache_test)
endif()
