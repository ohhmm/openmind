# Find hiredis library
# Sets the following variables:
# HIREDIS_FOUND - True if hiredis was found
# HIREDIS_INCLUDE_DIRS - hiredis include directories
# HIREDIS_LIBRARIES - hiredis libraries

find_path(HIREDIS_INCLUDE_DIR
    NAMES hiredis/hiredis.h
    PATHS /usr/include
          /usr/local/include
)
message(STATUS "HIREDIS_INCLUDE_DIR: ${HIREDIS_INCLUDE_DIR}")

find_library(HIREDIS_LIBRARY
    NAMES hiredis
    PATHS /usr/lib
          /usr/lib/x86_64-linux-gnu
          /usr/local/lib
)
message(STATUS "HIREDIS_LIBRARY: ${HIREDIS_LIBRARY}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(hiredis
    REQUIRED_VARS HIREDIS_LIBRARY HIREDIS_INCLUDE_DIR
)

if(HIREDIS_FOUND)
    set(HIREDIS_LIBRARIES ${HIREDIS_LIBRARY})
    set(HIREDIS_INCLUDE_DIRS ${HIREDIS_INCLUDE_DIR})
endif()

mark_as_advanced(HIREDIS_INCLUDE_DIR HIREDIS_LIBRARY)
