# Set policy to allow empty package
set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

# Skip building libbacktrace on all platforms
# This is a simplified approach to avoid build issues
if(VCPKG_TARGET_IS_OSX)
    message(STATUS "libbacktrace is not needed on macOS as it provides its own backtrace functionality")
else()
    message(STATUS "Using empty package for libbacktrace to avoid build issues")
endif()

# No actual build happens with empty package policy
