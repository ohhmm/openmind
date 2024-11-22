test()

if(OPENMIND_STORAGE_REDIS)
    target_link_libraries(redis_cache_test PRIVATE hiredis::hiredis)

    # Set default values for Redis test configuration
    if(NOT DEFINED ENV{OPENMIND_TEST_REDIS_HOST})
        if(WIN32)
            set(ENV{OPENMIND_TEST_REDIS_HOST} "127.0.0.1")
        else()
            set(ENV{OPENMIND_TEST_REDIS_HOST} "localhost")
        endif()
    endif()

    if(NOT DEFINED ENV{OPENMIND_TEST_REDIS_PORT})
        set(ENV{OPENMIND_TEST_REDIS_PORT} "6379")
    endif()

    # Set test environment variables
    set_tests_properties(redis_cache_test PROPERTIES
        ENVIRONMENT "OPENMIND_TEST_REDIS_HOST=$ENV{OPENMIND_TEST_REDIS_HOST};OPENMIND_TEST_REDIS_PORT=$ENV{OPENMIND_TEST_REDIS_PORT};OPENMIND_TEST_REDIS_RETRY_COUNT=3;OPENMIND_TEST_REDIS_RETRY_DELAY=1000"
        TIMEOUT 300
        LABELS "redis;storage"
        RESOURCE_LOCK redis
        FIXTURES_REQUIRED redis_running)

    # Add Redis service check fixture with platform-specific commands and retry logic
    if(WIN32)
        add_test(NAME redis_service_check
                COMMAND powershell -Command "
                    $retryCount = 3
                    $retryDelay = 2
                    while ($retryCount -gt 0) {
                        try {
                            $result = redis-cli -h $ENV:OPENMIND_TEST_REDIS_HOST ping
                            if ($result -eq 'PONG') { exit 0 }
                        } catch {}
                        $retryCount--
                        if ($retryCount -gt 0) { Start-Sleep -Seconds $retryDelay }
                    }
                    exit 1
                ")
    else()
        add_test(NAME redis_service_check
                COMMAND sh -c "
                    for i in $(seq 1 3); do
                        if redis-cli -h $ENV{OPENMIND_TEST_REDIS_HOST} ping | grep -q PONG; then
                            exit 0
                        fi
                        [ $i -lt 3 ] && sleep 2
                    done
                    exit 1
                ")
    endif()

    set_tests_properties(redis_service_check PROPERTIES
        FIXTURES_SETUP redis_running
        LABELS "redis;service")
endif()
