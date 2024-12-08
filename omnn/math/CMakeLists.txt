include(CMakeDependentOption)
cmake_dependent_option(OPENMIND_MATH_USE_FOUNDATIONDB_CACHE "Use Apple FoundationDB library to distribute calculations cache" ON "OPENMIND_STORAGE_FOUNDATIONDB" OFF)
cmake_dependent_option(OPENMIND_MATH_USE_LEVELDB_CACHE "Use Google LevelDB library to cache solutions" ON "OPENMIND_STORAGE_LEVELDB" OFF)

if(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
    option(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE "Update optimized cached values during fetch if new optimizations applied" ${OPENMIND_STORAGE_ALLOW_UPGRADE})
    option(OPENMIND_MATH_CACHE_VOLATILE "Clean cache each start (for debugging purposes)" ${OPENMIND_STORAGE_VOLATILE})
endif()

file(GLOB MATH_SOURCES
    "*.cpp"
    "*.h"
)

add_library(math STATIC ${MATH_SOURCES})
target_include_directories(math PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(DEPENDENCIES storage)
if(WIN32 AND OPENMIND_USE_VCPKG)
    list(APPEND DEPENDENCIES bcrypt)
endif()

target_link_libraries(math PUBLIC ${DEPENDENCIES})

if(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
    if(OPENMIND_MATH_USE_LEVELDB_CACHE)
        target_compile_definitions(math PUBLIC OPENMIND_MATH_USE_LEVELDB_CACHE)
    endif()
    if(OPENMIND_MATH_CACHE_VOLATILE)
        target_compile_definitions(math PUBLIC OPENMIND_MATH_CACHE_VOLATILE)
    endif()
    if(OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
        target_compile_definitions(math PUBLIC OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
    endif()
    if(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE)
        target_compile_definitions(math PUBLIC ALLOW_CACHE_UPGRADE)
    endif()
endif()

if(OPENMIND_BUILD_TESTS)
    add_subdirectory(test)
endif()
