set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

# Use empty package approach for all platforms to avoid build issues
if(VCPKG_TARGET_IS_OSX)
    message(STATUS "libbacktrace is not needed on macOS as it provides its own backtrace functionality")
else()
    message(STATUS "Using empty package for libbacktrace to avoid build issues")
endif()

# No actual build happens, just return with the empty package policy set
