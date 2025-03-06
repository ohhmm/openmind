set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

if(VCPKG_TARGET_IS_OSX)
    message(STATUS "libbacktrace is not needed on macOS as it provides its own backtrace functionality")
    # Return early without building anything
    return()
else()
    # For other platforms, use the regular port
    # This will delegate to the regular port for non-macOS platforms
    set(VCPKG_TARGET_IS_OSX_ORIGINAL ${VCPKG_TARGET_IS_OSX})
    set(VCPKG_TARGET_IS_OSX OFF)
    include(${CURRENT_INSTALLED_DIR}/share/vcpkg-cmake/vcpkg-port-config.cmake)
    include(${CURRENT_BUILTIN_PORT_DIR}/portfile.cmake)
    set(VCPKG_TARGET_IS_OSX ${VCPKG_TARGET_IS_OSX_ORIGINAL})
endif()
