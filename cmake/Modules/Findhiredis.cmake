# Find the hiredis libraries
#
# The following variables are optionally searched for defaults
#  HIREDIS_ROOT_DIR: Base directory where all hiredis components are found
#
# The following are set after configuration is done:
#  HIREDIS_FOUND
#  HIREDIS_INCLUDE_DIRS
#  HIREDIS_LIBRARIES

find_path(HIREDIS_INCLUDE_DIR
    NAMES hiredis/hiredis.h
    HINTS ${HIREDIS_ROOT_DIR}
    PATH_SUFFIXES include
)

if(HIREDIS_INCLUDE_DIR)
    file(STRINGS "${HIREDIS_INCLUDE_DIR}/hiredis/hiredis.h" HIREDIS_VERSION_MAJOR_LINE REGEX "^#define[ \t]+HIREDIS_MAJOR[ \t]+[0-9]+$")
    file(STRINGS "${HIREDIS_INCLUDE_DIR}/hiredis/hiredis.h" HIREDIS_VERSION_MINOR_LINE REGEX "^#define[ \t]+HIREDIS_MINOR[ \t]+[0-9]+$")
    file(STRINGS "${HIREDIS_INCLUDE_DIR}/hiredis/hiredis.h" HIREDIS_VERSION_PATCH_LINE REGEX "^#define[ \t]+HIREDIS_PATCH[ \t]+[0-9]+$")

    string(REGEX REPLACE "^#define[ \t]+HIREDIS_MAJOR[ \t]+([0-9]+)$" "\\1" HIREDIS_VERSION_MAJOR "${HIREDIS_VERSION_MAJOR_LINE}")
    string(REGEX REPLACE "^#define[ \t]+HIREDIS_MINOR[ \t]+([0-9]+)$" "\\1" HIREDIS_VERSION_MINOR "${HIREDIS_VERSION_MINOR_LINE}")
    string(REGEX REPLACE "^#define[ \t]+HIREDIS_PATCH[ \t]+([0-9]+)$" "\\1" HIREDIS_VERSION_PATCH "${HIREDIS_VERSION_PATCH_LINE}")

    set(HIREDIS_VERSION "${HIREDIS_VERSION_MAJOR}.${HIREDIS_VERSION_MINOR}.${HIREDIS_VERSION_PATCH}")
endif()

find_library(HIREDIS_LIBRARY
    NAMES hiredis
    HINTS ${HIREDIS_ROOT_DIR}
    PATH_SUFFIXES lib lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(hiredis
    REQUIRED_VARS
        HIREDIS_LIBRARY
        HIREDIS_INCLUDE_DIR
    VERSION_VAR HIREDIS_VERSION
)

# Enforce minimum version requirement
if(HIREDIS_FOUND AND HIREDIS_VERSION VERSION_LESS "0.13.0")
    message(FATAL_ERROR "hiredis version ${HIREDIS_VERSION} found, but version 0.13.0 or higher is required")
    set(HIREDIS_FOUND FALSE)
endif()

if(HIREDIS_FOUND)
    set(HIREDIS_INCLUDE_DIRS ${HIREDIS_INCLUDE_DIR})
    set(HIREDIS_LIBRARIES ${HIREDIS_LIBRARY})

    if(NOT TARGET hiredis::hiredis)
        add_library(hiredis::hiredis UNKNOWN IMPORTED)
        set_target_properties(hiredis::hiredis PROPERTIES
            IMPORTED_LOCATION "${HIREDIS_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${HIREDIS_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(
    HIREDIS_ROOT_DIR
    HIREDIS_LIBRARY
    HIREDIS_INCLUDE_DIR
)
