# Set policy to allow empty package
set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

# Skip building libbacktrace on all platforms
if(VCPKG_TARGET_IS_OSX)
    message(STATUS "libbacktrace is not needed on macOS as it provides its own backtrace functionality")
else()
    message(STATUS "Using empty package for libbacktrace to avoid build issues")
endif()

# Explicitly install an empty package
file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/share/libbacktrace)
file(WRITE ${CURRENT_PACKAGES_DIR}/share/libbacktrace/copyright "See https://github.com/ianlancetaylor/libbacktrace for license information.")
