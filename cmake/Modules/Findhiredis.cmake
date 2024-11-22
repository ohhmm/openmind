# Find hiredis library
# Sets the following variables:
# HIREDIS_FOUND - True if hiredis was found
# HIREDIS_INCLUDE_DIRS - hiredis include directories
# HIREDIS_LIBRARIES - hiredis libraries

# Try pkg-config first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_HIREDIS QUIET hiredis)
endif()

# Search for header in common system paths and Conan paths
find_path(HIREDIS_INCLUDE_DIR
    NAMES hiredis/hiredis.h
    HINTS
        ${PC_HIREDIS_INCLUDEDIR}
        ${PC_HIREDIS_INCLUDE_DIRS}
        ${CONAN_INCLUDE_DIRS_HIREDIS}
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /opt/homebrew/include
    PATH_SUFFIXES hiredis
)

# Search for library in common system paths and Conan paths
find_library(HIREDIS_LIBRARY
    NAMES hiredis libhiredis
    HINTS
        ${PC_HIREDIS_LIBDIR}
        ${PC_HIREDIS_LIBRARY_DIRS}
        ${CONAN_LIB_DIRS_HIREDIS}
    PATHS
        /usr/lib
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib
        /opt/local/lib
        /opt/homebrew/lib
)

# Debug output
message(STATUS "HIREDIS_INCLUDE_DIR: ${HIREDIS_INCLUDE_DIR}")
message(STATUS "HIREDIS_LIBRARY: ${HIREDIS_LIBRARY}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(hiredis
    REQUIRED_VARS HIREDIS_LIBRARY HIREDIS_INCLUDE_DIR
    VERSION_VAR PC_HIREDIS_VERSION
)

if(HIREDIS_FOUND AND NOT TARGET hiredis::hiredis)
    add_library(hiredis::hiredis UNKNOWN IMPORTED)
    set_target_properties(hiredis::hiredis PROPERTIES
        IMPORTED_LOCATION "${HIREDIS_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${HIREDIS_INCLUDE_DIR}")
endif()

if(HIREDIS_FOUND)
    set(HIREDIS_LIBRARIES ${HIREDIS_LIBRARY})
    set(HIREDIS_INCLUDE_DIRS ${HIREDIS_INCLUDE_DIR})
endif()

mark_as_advanced(HIREDIS_INCLUDE_DIR HIREDIS_LIBRARY)
