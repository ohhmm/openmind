include(CMakeDependentOption)
cmake_dependent_option(OPENMIND_MATH_USE_FOUNDATIONDB_CACHE "Use Apple FoundationDB library to distribute calculations cache" ON "OPENMIND_STORAGE_FOUNDATIONDB" OFF)
cmake_dependent_option(OPENMIND_MATH_USE_LEVELDB_CACHE "Use Google LevelDB library to cache solutions" ON "OPENMIND_STORAGE_LEVELDB" OFF)

if(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
    option(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE "Update optimized cached values during fetch if new optimizations applied" ${OPENMIND_STORAGE_ALLOW_UPGRADE})
    option(OPENMIND_MATH_CACHE_VOLATILE "Clean cache each start (for debugging purposes)" ${OPENMIND_STORAGE_VOLATILE})
endif(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)

set(DEPENDENCIES storage)
if(WIN32 AND OPENMIND_USE_VCPKG)
    list(APPEND DEPENDENCIES bcrypt) # workaround boost-uuid vcpkg bug
endif()

# Create library with dependencies
lib(${DEPENDENCIES})
if(WIN32)
    find_package(leveldb CONFIG REQUIRED)
    target_link_libraries(${this_target} PUBLIC leveldb::leveldb)
else()
    target_link_libraries(${this_target} PUBLIC leveldb)
endif()

if(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
    if(OPENMIND_MATH_USE_LEVELDB_CACHE)
        target_compile_definitions(${this_target} PUBLIC OPENMIND_MATH_USE_LEVELDB_CACHE)
    endif()
    if(OPENMIND_MATH_CACHE_VOLATILE)
        target_compile_definitions(${this_target} PUBLIC OPENMIND_MATH_CACHE_VOLATILE)
    endif(OPENMIND_MATH_CACHE_VOLATILE)
    if(OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
        target_compile_definitions(${this_target} PUBLIC OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
    endif()
    if(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE)
        target_compile_definitions(${this_target} PUBLIC ALLOW_CACHE_UPGRADE)
    endif()
endif()

#add_library(OpenMind::math ALIAS math)
