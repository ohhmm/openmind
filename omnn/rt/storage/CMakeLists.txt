# Storage module CMake configuration
set(DEPENDENCIES
    rt
)

# Add Redis dependency
if(OPENMIND_STORAGE_REDIS)
    find_package(hiredis REQUIRED)
    list(APPEND DEPENDENCIES hiredis::hiredis)
    add_definitions(-DOPENMIND_STORAGE_REDIS)

    if(WIN32)
        # On Windows, default to Memurai if available
        find_program(MEMURAI_EXECUTABLE "memurai-developer.exe"
            PATHS "C:/Program Files/Memurai"
            DOC "Path to Memurai Developer executable")
        if(MEMURAI_EXECUTABLE OR OPENMIND_STORAGE_REDIS_MEMURAI)
            add_definitions(-DOPENMIND_STORAGE_REDIS_MEMURAI)
            message(STATUS "Using Memurai as Redis provider on Windows")
        else()
            message(STATUS "Using Redis on Windows (Memurai not found)")
        endif()
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
    add_subdirectory(tests)
endif()
