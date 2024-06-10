include(CMakeDependentOption)
cmake_dependent_option(OPENMIND_MATH_USE_FOUNDATIONDB_CACHE "Use Apple FoundationDB library to distribute calculations cache" ON "OPENMIND_STORAGE_FOUNDATIONDB" OFF)
cmake_dependent_option(OPENMIND_MATH_USE_LEVELDB_CACHE "Use Google LevelDB library to cache solutions" ON "OPENMIND_STORAGE_LEVELDB" OFF)

if(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
    option(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE "Update optimized cached values during fetch if new optimizations applied" ${OPENMIND_STORAGE_ALLOW_UPGRADE})
    option(OPENMIND_MATH_CACHE_VOLATILE "Clean cache each start (for debugging purposes)" ${OPENMIND_STORAGE_VOLATILE})
endif(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)

lib(storage)

# Manually add source files for Valuable class and its descendants
list(APPEND src
    ${CMAKE_CURRENT_SOURCE_DIR}/Valuable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Variable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Integer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Exponentiation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Fraction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Sum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Modulo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Product.cpp
)

add_library(math_lib ${src} ${headers})
apply_target_commons(math_lib)

if(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
    if(OPENMIND_MATH_USE_LEVELDB_CACHE)
        target_compile_definitions(math_lib PUBLIC OPENMIND_MATH_USE_LEVELDB_CACHE)
    endif()
    if(OPENMIND_MATH_CACHE_VOLATILE)
        target_compile_definitions(math_lib PUBLIC OPENMIND_MATH_CACHE_VOLATILE)
    endif(OPENMIND_MATH_CACHE_VOLATILE)
    if(OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
        target_compile_definitions(math_lib PUBLIC OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
    endif()
    if(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE)
        target_compile_definitions(math_lib PUBLIC ALLOW_CACHE_UPGRADE)
    endif()
endif()

# Add include directories for header files
target_include_directories(math_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../rt)

# Ensure the current source directory is included in the search path for include files
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Link the math_lib target with the rt library to ensure include paths are inherited
target_link_libraries(math_lib PUBLIC rt Boost::log Boost::log_setup)
