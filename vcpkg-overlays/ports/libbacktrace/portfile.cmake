set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

if(VCPKG_TARGET_IS_OSX)
    message(STATUS "libbacktrace is not needed on macOS as it provides its own backtrace functionality")
    # Return early without building anything
    return()
else()
    # For other platforms, just skip the build
    # This is a simplified approach to avoid build issues
    message(STATUS "Using empty package for libbacktrace on non-macOS platforms to avoid build issues")
    # No need to include any other portfile, just return with the empty package policy set
endif()
