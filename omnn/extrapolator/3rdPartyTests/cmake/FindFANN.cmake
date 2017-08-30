
IF(NOT WIN32)
    FIND_PATH(FANN_INCLUDE_DIR fann.h
            PATHS
            /usr/include
            /opt/local/include
            )
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
        FIND_LIBRARY(FANN_LIBRARIES
                NAMES fann
                PATHS
                    /usr/lib64
                    /opt/local/lib
                )
    ELSE()
        FIND_LIBRARY(FANN_LIBRARIES
                NAMES fann
                PATHS
                    /usr/lib
                    /opt/local/lib
                )
    ENDIF()
ENDIF()

IF(FANN_INCLUDE_DIR)
    SET(FANN_FOUND 1)
    IF(NOT FANN_FOUND_QUIETLY)
        MESSAGE(STATUS "Found FANN: include = ${FANN_INCLUDE_DIR}, libraries = ${FANN_LIBRARIES}")
    ENDIF()
ELSE()
    SET(FANN_FOUND 0 CACHE BOOL "FANN not found")
    MESSAGE(STATUS "FANN not found, disabled")
ENDIF()

MARK_AS_ADVANCED(FANN_INCLUDE_DIR FANN_LIBRARIES)
