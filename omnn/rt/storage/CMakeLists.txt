# Storage module CMake configuration
set(DEPENDENCIES
    rt
)

# Add Redis dependency
if(WIN32)
    # On Windows, default to Memurai
    option(OPENMIND_STORAGE_REDIS "Enable Redis/Memurai storage backend" ON)
    if(OPENMIND_STORAGE_REDIS)
        find_package(hiredis REQUIRED)
        list(APPEND DEPENDENCIES hiredis::hiredis)
        add_definitions(-DOPENMIND_STORAGE_REDIS)

        # Check for Memurai
        find_program(MEMURAI_EXECUTABLE "memurai-developer.exe"
            PATHS
            "C:/Program Files/Memurai Developer"
            "C:/Program Files/Memurai"
            "C:/Program Files (x86)/Memurai Developer"
            "C:/Program Files (x86)/Memurai"
            ENV PATH
            DOC "Path to Memurai Developer executable")

        if(MEMURAI_EXECUTABLE OR OPENMIND_STORAGE_REDIS_MEMURAI)
            add_definitions(-DOPENMIND_STORAGE_REDIS_MEMURAI)
            message(STATUS "Using Memurai as Redis provider on Windows at: ${MEMURAI_EXECUTABLE}")
        else()
            message(STATUS "Searching for Memurai in alternative locations...")
            file(GLOB_RECURSE MEMURAI_SEARCH
                "C:/Program Files/**/memurai-developer.exe"
                "C:/Program Files (x86)/**/memurai-developer.exe")

            if(MEMURAI_SEARCH)
                list(GET MEMURAI_SEARCH 0 MEMURAI_EXECUTABLE)
                add_definitions(-DOPENMIND_STORAGE_REDIS_MEMURAI)
                message(STATUS "Found Memurai via search at: ${MEMURAI_EXECUTABLE}")
            else()
                message(FATAL_ERROR "Memurai not found. Please ensure Memurai Developer is installed correctly and the executable is accessible.")
            endif()
        endif()
    endif()
else()
    # Non-Windows platforms use standard Redis
    option(OPENMIND_STORAGE_REDIS "Enable Redis storage backend" ON)
    if(OPENMIND_STORAGE_REDIS)
        find_package(hiredis REQUIRED)
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
    add_subdirectory(tests)
endif()
