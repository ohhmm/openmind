# Windows toolchain file for OpenMind

# System settings
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})

# Force static runtime for MSVC
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

# Boost configuration
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_NO_SYSTEM_PATHS ON)
set(BOOST_ROOT "$ENV{VCPKG_ROOT}/installed/x64-windows")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
set(Boost_DIR "${BOOST_ROOT}/share/boost")

# Module paths
list(APPEND CMAKE_MODULE_PATH
    "$ENV{VCPKG_ROOT}/installed/x64-windows/share"
    "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg/ports"
)

# OpenCL configuration
if(OPENMIND_USE_OPENCL)
    set(OpenCL_INCLUDE_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows/include")
    set(OpenCL_LIBRARY "$ENV{VCPKG_ROOT}/installed/x64-windows/lib/OpenCL.lib")
    set(OpenCL_LIBRARY_DEBUG "$ENV{VCPKG_ROOT}/installed/x64-windows/debug/lib/OpenCL.lib")
    set(OpenCL_VERSION_STRING "3.0")
    set(OpenCL_FOUND TRUE)
endif()

# LevelDB configuration
set(LEVELDB_BUILD_TESTS OFF)
set(LEVELDB_BUILD_BENCHMARKS OFF)
set(LEVELDB_INSTALL OFF)

# Compiler flags
if(MSVC)
    add_compile_options(/MP /W4 /EHsc)
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
endif()

# Set vcpkg triplet
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
endif()
