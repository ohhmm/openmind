# Storage module CMake configuration
set(DEPENDENCIES
    rt
)

# Add Redis dependency
if(WIN32)
    # On Windows, default to Memurai
    option(OPENMIND_STORAGE_REDIS "Enable Redis/Memurai storage backend" ON)
    if(OPENMIND_STORAGE_REDIS)
        find_package(hiredis REQUIRED CONFIG)
        list(APPEND DEPENDENCIES hiredis::hiredis)
        add_definitions(-DOPENMIND_STORAGE_REDIS)

        # Enhanced Memurai detection
        find_program(MEMURAI_EXECUTABLE "memurai-developer.exe"
            PATHS
            "C:/Program Files/Memurai Developer"
            "C:/Program Files/Memurai"
            "C:/Program Files (x86)/Memurai Developer"
            "C:/Program Files (x86)/Memurai"
            ENV PATH
            NO_CACHE)

        if(NOT MEMURAI_EXECUTABLE)
            message(STATUS "Searching for Memurai in Windows Registry...")
            execute_process(
                COMMAND reg query "HKLM\\SOFTWARE\\Memurai\\Developer" /v InstallLocation
                OUTPUT_VARIABLE MEMURAI_REG_OUT
                ERROR_VARIABLE MEMURAI_REG_ERR
                RESULT_VARIABLE MEMURAI_REG_RESULT)

            if(MEMURAI_REG_RESULT EQUAL 0)
                string(REGEX MATCH "InstallLocation[ ]*REG_SZ[ ]*([^\r\n]*)" MEMURAI_PATH "${MEMURAI_REG_OUT}")
                if(CMAKE_MATCH_1)
                    set(MEMURAI_EXECUTABLE "${CMAKE_MATCH_1}/memurai-developer.exe")
                endif()
            endif()
        endif()

        if(MEMURAI_EXECUTABLE OR OPENMIND_STORAGE_REDIS_MEMURAI)
            add_definitions(-DOPENMIND_STORAGE_REDIS_MEMURAI)
            message(STATUS "Using Memurai as Redis provider on Windows at: ${MEMURAI_EXECUTABLE}")

            # Verify Memurai service status and functionality
            execute_process(
                COMMAND powershell -Command "
                    $status = Get-Service memurai -ErrorAction SilentlyContinue;
                    if ($status) {
                        Write-Host \"Service Status: $($status.Status)\";
                        if ($status.Status -ne 'Running') {
                            Write-Host \"Starting Memurai service...\";
                            Start-Service memurai;
                            Start-Sleep -s 5;
                            $status = Get-Service memurai;
                        }
                        exit $($status.Status -eq 'Running' ? 0 : 1);
                    } else {
                        Write-Host \"Memurai service not found\";
                        exit 2;
                    }"
                RESULT_VARIABLE MEMURAI_SERVICE_RESULT
                OUTPUT_VARIABLE MEMURAI_SERVICE_OUTPUT)

            if(NOT MEMURAI_SERVICE_RESULT EQUAL 0)
                message(FATAL_ERROR "Memurai service verification failed:\n${MEMURAI_SERVICE_OUTPUT}\nPlease ensure Memurai is properly installed and the service is running.")
            endif()
        else()
            message(FATAL_ERROR "Memurai not found. Please install Memurai Developer from https://www.memurai.com/get-memurai")
        endif()
    endif()
else()
    # Non-Windows platforms use standard Redis
    option(OPENMIND_STORAGE_REDIS "Enable Redis storage backend" ON)
    if(OPENMIND_STORAGE_REDIS)
        # First try to find hiredis through our custom module
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
        find_package(hiredis MODULE REQUIRED)

        if(NOT HIREDIS_FOUND)
            message(FATAL_ERROR "hiredis library not found. Please install libhiredis-dev package.")
        endif()

        # Add to dependencies regardless - it will be installed by prerequisites
        list(APPEND DEPENDENCIES hiredis::hiredis)
        add_definitions(-DOPENMIND_STORAGE_REDIS)
    endif()
endif()

# Add LevelDB dependency
if(OPENMIND_STORAGE_LEVELDB)
    find_package(leveldb REQUIRED)
    list(APPEND DEPENDENCIES leveldb)
    add_definitions(-DOPENMIND_STORAGE_LEVELDB)
endif()

lib(${DEPENDENCIES})

if(OPENMIND_BUILD_TESTS)
    add_subdirectory(tests ${CMAKE_CURRENT_BINARY_DIR}/storage_tests)
endif()
