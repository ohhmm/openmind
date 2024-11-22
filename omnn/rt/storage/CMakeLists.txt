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
        add_definitions(-DOPENMIND_STORAGE_REDIS_MEMURAI)
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
