set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

if(VCPKG_TARGET_IS_OSX)
    message(STATUS "libbacktrace is not needed on macOS as it provides its own backtrace functionality")
    # Return early without building anything
    return()
else()
    # For other platforms, use the regular port
    # This will delegate to the regular port for non-macOS platforms
    # Use the builtin port directly
    include(${CMAKE_CURRENT_LIST_DIR}/../../libbacktrace/portfile.cmake)
endif()
