option(OPENMIND_STORAGE_FOUNDATIONDB "Use Apple FoundationDB library for one of the multistorage levels" NO)
option(OPENMIND_STORAGE_LEVELDB "Use Google LevelDB library for one of the multistorage levels" ON)
option(OPENMIND_STORAGE_REDIS "Use Redis for distributed caching" OFF)

set(DEPENDENCIES rt)
if(OPENMIND_STORAGE_LEVELDB OR OPENMIND_STORAGE_FOUNDATIONDB OR OPENMIND_STORAGE_REDIS)
    option(OPENMIND_STORAGE_ALLOW_UPGRADE "Update optimized cached values during fetch if new optimizations applied" ON)
    option(OPENMIND_STORAGE_VOLATILE "Clean cache each start (for debugging purposes)" NO)

    if(OPENMIND_STORAGE_LEVELDB)
        find_package(Threads) # workaround leveldb config bug
        list(APPEND DEPENDENCIES google/leveldb)
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

# Add cache_base library
add_library(cache_base
    CacheBase.cpp
    CacheBase.h
)

target_link_libraries(cache_base
    PUBLIC
    ${DEPENDENCIES}
    Boost::filesystem
    Boost::system
    ${CMAKE_DL_LIBS}
    -lboost_filesystem
    -lboost_system
)

target_include_directories(cache_base
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

lib(${DEPENDENCIES})

if(OPENMIND_STORAGE_LEVELDB)
    target_compile_definitions(${this_target} PUBLIC OPENMIND_STORAGE_LEVELDB)
endif()
if(OPENMIND_STORAGE_FOUNDATIONDB)
    target_compile_definitions(${this_target} PUBLIC OPENMIND_STORAGE_FOUNDATIONDB)
endif()
if(OPENMIND_STORAGE_REDIS)
    target_compile_definitions(${this_target} PUBLIC OPENMIND_STORAGE_REDIS)
endif()
if(OPENMIND_STORAGE_ALLOW_UPGRADE)
    target_compile_definitions(${this_target} PUBLIC ALLOW_UPGRADE OPENMIND_STORAGE_ALLOW_UPGRADE)
endif()
