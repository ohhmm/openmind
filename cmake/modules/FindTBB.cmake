# Find Intel Threading Building Blocks (TBB)
include(FindPackageHandleStandardArgs)

if(DEFINED ENV{CI})
    message(STATUS "CI environment detected, using vcpkg TBB")
    
    if(DEFINED ENV{VCPKG_ROOT})
        if(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
            set(VCPKG_TBB_TRIPLET "arm64-osx")
        elseif(DEFINED ENV{VCPKG_DEFAULT_TRIPLET})
            set(VCPKG_TBB_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}")
        else()
            if(APPLE)
                set(VCPKG_TBB_TRIPLET "x64-osx")
            elseif(WIN32)
                set(VCPKG_TBB_TRIPLET "x64-windows")
            else()
                set(VCPKG_TBB_TRIPLET "x64-linux")
            endif()
        endif()
        
        message(STATUS "VCPKG_ROOT: $ENV{VCPKG_ROOT}")
        message(STATUS "VCPKG_TBB_TRIPLET: ${VCPKG_TBB_TRIPLET}")
        
        get_filename_component(VCPKG_ROOT "$ENV{VCPKG_ROOT}" ABSOLUTE)
        set(VCPKG_INSTALLED_DIR "${VCPKG_ROOT}/installed/${VCPKG_TBB_TRIPLET}")
        set(TBB_INCLUDE_DIRS "${VCPKG_INSTALLED_DIR}/include")
        set(TBB_LIBRARY_DIR "${VCPKG_INSTALLED_DIR}/lib")
        
        message(STATUS "Checking vcpkg directories:")
        set(EXISTS_VCPKG_ROOT EXISTS "${VCPKG_ROOT}")
        set(EXISTS_VCPKG_INSTALLED EXISTS "${VCPKG_INSTALLED_DIR}")
        set(EXISTS_TBB_INCLUDE EXISTS "${TBB_INCLUDE_DIRS}")
        set(EXISTS_TBB_LIB EXISTS "${TBB_LIBRARY_DIR}")
        
        message(STATUS "  VCPKG_ROOT: ${VCPKG_ROOT} (exists: ${EXISTS_VCPKG_ROOT})")
        message(STATUS "  VCPKG_INSTALLED_DIR: ${VCPKG_INSTALLED_DIR} (exists: ${EXISTS_VCPKG_INSTALLED})")
        message(STATUS "  Include dir: ${TBB_INCLUDE_DIRS} (exists: ${EXISTS_TBB_INCLUDE})")
        message(STATUS "  Library dir: ${TBB_LIBRARY_DIR} (exists: ${EXISTS_TBB_LIB})")
        
        if(NOT EXISTS "${VCPKG_ROOT}")
            message(FATAL_ERROR "VCPKG_ROOT directory not found: ${VCPKG_ROOT}")
        endif()
        
        if(NOT EXISTS "${VCPKG_INSTALLED_DIR}")
            message(FATAL_ERROR "VCPKG installed directory not found: ${VCPKG_INSTALLED_DIR}")
        endif()
        
        if(NOT EXISTS "${TBB_INCLUDE_DIRS}")
            message(FATAL_ERROR "TBB include directory not found: ${TBB_INCLUDE_DIRS}")
        endif()
        
        if(NOT EXISTS "${TBB_LIBRARY_DIR}")
            message(FATAL_ERROR "TBB library directory not found: ${TBB_LIBRARY_DIR}")
        endif()
        
        set(TBB_FOUND TRUE)
        
        file(TO_CMAKE_PATH "$ENV{VCPKG_ROOT}/installed/${VCPKG_TBB_TRIPLET}" VCPKG_INSTALLED_DIR)
        file(TO_CMAKE_PATH "${VCPKG_INSTALLED_DIR}/include" TBB_INCLUDE_DIRS)
        file(TO_CMAKE_PATH "${VCPKG_INSTALLED_DIR}/lib" TBB_LIBRARY_DIR)
        
        find_library(TBB_LIBRARY
            NAMES tbb libtbb tbb12 libtbb12
            PATHS 
                "${TBB_LIBRARY_DIR}"
                "${VCPKG_INSTALLED_DIR}/lib"
                /usr/local/lib
                /opt/homebrew/lib
            NO_DEFAULT_PATH
        )
        
        if(TBB_LIBRARY)
            set(TBB_LIBRARIES "${TBB_LIBRARY}")
            message(STATUS "Found TBB: ${TBB_LIBRARY}")
            
            if(NOT TARGET TBB::tbb)
                add_library(TBB::tbb UNKNOWN IMPORTED GLOBAL)
                set_target_properties(TBB::tbb PROPERTIES
                    IMPORTED_LOCATION "${TBB_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${TBB_INCLUDE_DIRS}"
                    IMPORTED_NO_SONAME TRUE
                )
                
                # Add RPATH settings for macOS
                if(APPLE)
                    set_target_properties(TBB::tbb PROPERTIES
                        INSTALL_RPATH "@loader_path/../lib"
                        BUILD_WITH_INSTALL_RPATH TRUE
                    )
                endif()
                message(STATUS "Created TBB::tbb target (CI) -> ${TBB_LIBRARY}")
                message(STATUS "  Include dirs: ${TBB_INCLUDE_DIRS}")
                message(STATUS "  Library path: ${TBB_LIBRARY}")
            endif()
        else()
            message(WARNING "TBB library not found in vcpkg or system paths")
            message(STATUS "Searched paths:")
            message(STATUS "  ${TBB_LIBRARY_DIR}")
            message(STATUS "  ${VCPKG_INSTALLED_DIR}/lib")
            message(STATUS "  /usr/local/lib")
            message(STATUS "  /opt/homebrew/lib")
            if(NOT TARGET TBB::tbb)
                add_library(TBB::tbb INTERFACE IMPORTED)
            endif()
        endif()
    endif()
    return()
endif()

if(NOT TBB_FOUND)
    # First try to find TBB using the system's package config
    find_package(TBB QUIET CONFIG)
    
    if(NOT TBB_FOUND)
        # If not found via config, search for the components manually
        find_path(TBB_INCLUDE_DIR
            NAMES tbb/tbb.h oneapi/tbb.h
            PATHS
                ${TBB_ROOT}/include
                $ENV{TBB_ROOT}/include
                /usr/include
                /usr/local/include
                /opt/homebrew/include
                "C:/Program Files/Intel/TBB/include"
                "C:/Program Files (x86)/Intel/TBB/include"
                "C:/Program Files/oneAPI/tbb/latest/include"
        )

        find_library(TBB_LIBRARY
            NAMES tbb
            PATHS
                ${TBB_ROOT}/lib
                ${TBB_ROOT}/lib64
                $ENV{TBB_ROOT}/lib
                $ENV{TBB_ROOT}/lib64
                /usr/lib
                /usr/lib64
                /usr/local/lib
                /usr/local/lib64
                /opt/homebrew/lib
        )

        find_package_handle_standard_args(TBB
            REQUIRED_VARS 
                TBB_LIBRARY 
                TBB_INCLUDE_DIR
        )

        if(TBB_FOUND)
            set(TBB_INCLUDE_DIRS ${TBB_INCLUDE_DIR})
            set(TBB_LIBRARIES ${TBB_LIBRARY})
            
            if(NOT TARGET TBB::tbb)
                add_library(TBB::tbb SHARED IMPORTED GLOBAL)
                set_target_properties(TBB::tbb PROPERTIES
                    IMPORTED_LOCATION "${TBB_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${TBB_INCLUDE_DIR}"
                )
                message(STATUS "Created TBB::tbb target (non-CI) -> ${TBB_LIBRARY}")
                message(STATUS "  Include dirs: ${TBB_INCLUDE_DIR}")
                message(STATUS "  Library path: ${TBB_LIBRARY}")
            endif()
        endif()
    endif()
endif()

mark_as_advanced(TBB_INCLUDE_DIR TBB_LIBRARY)

if(TBB_FOUND)
    message(STATUS "Found TBB: ${TBB_LIBRARIES}")
else()
    message(STATUS "Could not find TBB")
    message(STATUS "TBB_ROOT: ${TBB_ROOT}")
    message(STATUS "TBB_INCLUDE_DIR: ${TBB_INCLUDE_DIR}")
    message(STATUS "TBB_LIBRARY: ${TBB_LIBRARY}")
endif()
