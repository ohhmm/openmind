option(OPENMIND_STORAGE_FOUNDATIONDB "Use Apple FoundationDB library for one of the multistorage levels" NO)
option(OPENMIND_STORAGE_LEVELDB "Use Google LevelDB library for one of the multistorage levels" ON)

set(STORAGE_SOURCES
    Storage.cpp
    StorageBase.cpp
    MultiStorage.cpp
)

set(DEPENDENCIES rt)

# Include Conan-generated configs if available
include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL)

if(OPENMIND_STORAGE_LEVELDB OR OPENMIND_STORAGE_FOUNDATIONDB)
    option(OPENMIND_STORAGE_ALLOW_UPGRADE "Update optimized cached values during fetch if new optimizations applied" ON)
    option(OPENMIND_STORAGE_VOLATILE "Clean cache each start (for debugging purposes)" NO)

    if(OPENMIND_STORAGE_LEVELDB)
        find_package(Threads) # workaround leveldb config bug
        list(APPEND DEPENDENCIES leveldb)  # Changed from google/leveldb to leveldb
        if(NOT WIN32)
            list(APPEND DEPENDENCIES pthread)
        endif()
    endif()

    if(OPENMIND_STORAGE_FOUNDATIONDB)
        list(APPEND DEPENDENCIES ohhmm/FoundationDB)
    endif()

    if(OPENMIND_STORAGE_REDIS)
        list(APPEND DEPENDENCIES Boost::redis)
    endif()

endif(OPENMIND_STORAGE_LEVELDB OR OPENMIND_STORAGE_FOUNDATIONDB OR OPENMIND_STORAGE_REDIS)

lib(${STORAGE_SOURCES} ${DEPENDENCIES})

if(OPENMIND_STORAGE_LEVELDB)
    target_compile_definitions(${this_target} PUBLIC OPENMIND_STORAGE_LEVELDB)
endif()
if(OPENMIND_STORAGE_FOUNDATIONDB)
    target_compile_definitions(${this_target} PUBLIC OPENMIND_STORAGE_FOUNDATIONDB)
endif()
if(OPENMIND_STORAGE_ALLOW_UPGRADE)
    target_compile_definitions(${this_target} PUBLIC ALLOW_UPGRADE OPENMIND_STORAGE_ALLOW_UPGRADE)
endif()

# Tests are now handled in rt/storage/tests
# if(OPENMIND_BUILD_TESTS)
#     add_subdirectory(test)
# endif()
