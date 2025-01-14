# Find Boost
include(FindPackageHandleStandardArgs)

# Handle CI environment
if(DEFINED ENV{CI})
    message(STATUS "CI environment detected, using vcpkg Boost")
    
    # Determine vcpkg triplet
    if(APPLE)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
            set(VCPKG_TRIPLET "arm64-osx")
        else()
            set(VCPKG_TRIPLET "x64-osx")
        endif()
    elseif(WIN32)
        set(VCPKG_TRIPLET "x64-windows")
    else()
        set(VCPKG_TRIPLET "x64-linux")
    endif()
    
    # Override with environment variable if set
    if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET})
        set(VCPKG_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}")
    endif()
    
    message(STATUS "Using vcpkg triplet: ${VCPKG_TRIPLET}")
    
    # Set up vcpkg paths
    if(DEFINED ENV{VCPKG_ROOT})
        get_filename_component(VCPKG_ROOT "$ENV{VCPKG_ROOT}" ABSOLUTE)
        set(VCPKG_INSTALLED_DIR "${VCPKG_ROOT}/installed/${VCPKG_TRIPLET}")
        set(Boost_INCLUDE_DIRS "${VCPKG_INSTALLED_DIR}/include")
        set(Boost_LIBRARY_DIRS "${VCPKG_INSTALLED_DIR}/lib")
        
        message(STATUS "Checking vcpkg directories:")
        message(STATUS "  VCPKG_ROOT: ${VCPKG_ROOT}")
        message(STATUS "  VCPKG_INSTALLED_DIR: ${VCPKG_INSTALLED_DIR}")
        message(STATUS "  Include dir: ${Boost_INCLUDE_DIRS}")
        message(STATUS "  Library dir: ${Boost_LIBRARY_DIRS}")
        
        # Only verify VCPKG_ROOT exists initially
        if(NOT EXISTS "${VCPKG_ROOT}")
            message(FATAL_ERROR "VCPKG_ROOT directory not found: ${VCPKG_ROOT}")
        endif()
        
        # Other directories will be created during vcpkg install
        if(EXISTS "${VCPKG_INSTALLED_DIR}")
            # Verify include and lib dirs only if installation directory exists
            foreach(DIR IN ITEMS "${Boost_INCLUDE_DIRS}" "${Boost_LIBRARY_DIRS}")
                if(NOT EXISTS "${DIR}")
                    message(WARNING "Directory not found, will be created by vcpkg: ${DIR}")
                endif()
            endforeach()
        endif()
        
        # Configure Boost
        set(Boost_FOUND TRUE)
        set(Boost_VERSION ${OPENMIND_REQUIRED_BOOST_VERSION})
        set(Boost_DIR "${VCPKG_INSTALLED_DIR}")
        
        # Create header target
        if(NOT TARGET Boost::headers)
            add_library(Boost::headers INTERFACE IMPORTED GLOBAL)
            set_target_properties(Boost::headers PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
            )
            message(STATUS "Created Boost::headers target -> ${Boost_INCLUDE_DIRS}")
        endif()
        
        # Define required components
        set(REQUIRED_COMPONENTS
            chrono
            unit_test_framework
            filesystem
            system
            thread
            serialization
            multiprecision
            compute
            test_exec_monitor
        )
        
        # Create component targets
        foreach(component IN LISTS REQUIRED_COMPONENTS)
            string(REPLACE "boost_" "" lib_name "${component}")
            set(lib_path "${Boost_LIBRARY_DIRS}/libboost_${lib_name}")
            
            # Find library file with various naming patterns
            set(lib_file "")
            if(APPLE)
                foreach(suffix IN ITEMS
                    ".dylib"
                    "-mt.dylib"
                    ".a"
                    "-mt.a"
                    "-mt-s.a"
                    )
                    if(EXISTS "${lib_path}${suffix}")
                        set(lib_file "${lib_path}${suffix}")
                        if(suffix MATCHES ".*\\.dylib")
                            set(lib_type SHARED)
                        else()
                            set(lib_type STATIC)
                        endif()
                        break()
                    endif()
                endforeach()
                
                # Debug output for library search
                if(NOT lib_file)
                    message(STATUS "Searching for Boost::${component} library:")
                    message(STATUS "  Base path: ${lib_path}")
                    foreach(suffix IN ITEMS ".dylib" "-mt.dylib" ".a" "-mt.a" "-mt-s.a")
                        message(STATUS "  Trying: ${lib_path}${suffix}")
                        if(EXISTS "${lib_path}${suffix}")
                            message(STATUS "    Found!")
                        endif()
                    endforeach()
                endif()
            else()
                foreach(suffix IN ITEMS
                    ".so"
                    "-mt.so"
                    ".a"
                    "-mt.a"
                    )
                    if(EXISTS "${lib_path}${suffix}")
                        set(lib_file "${lib_path}${suffix}")
                        if(suffix MATCHES ".*\\.so")
                            set(lib_type SHARED)
                        else()
                            set(lib_type STATIC)
                        endif()
                        break()
                    endif()
                endforeach()
            endif()
            
            if(NOT lib_file)
                message(FATAL_ERROR "Could not find library for Boost::${component} at ${lib_path} (tried various suffixes)")
            endif()
            
            # Create target
            if(NOT TARGET Boost::${component})
                add_library(Boost::${component} ${lib_type} IMPORTED GLOBAL)
                set_target_properties(Boost::${component} PROPERTIES
                    IMPORTED_LOCATION "${lib_file}"
                    INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
                    IMPORTED_NO_SONAME TRUE
                )
                
                # Add RPATH settings for macOS
                if(APPLE AND lib_type STREQUAL "SHARED")
                    set_target_properties(Boost::${component} PROPERTIES
                        INSTALL_RPATH "@loader_path/../lib"
                        BUILD_WITH_INSTALL_RPATH TRUE
                    )
                endif()
                
                message(STATUS "Created Boost::${component} target (${lib_type}) -> ${lib_file}")
            endif()
        endforeach()
        
        message(STATUS "Boost configuration complete")
        return()
    else()
        # Try to find vcpkg root in various locations
        if(DEFINED ENV{GITHUB_WORKSPACE})
            # In GitHub Actions, vcpkg is installed in the workspace
            set(VCPKG_ROOT "$ENV{GITHUB_WORKSPACE}/vcpkg")
            message(STATUS "GitHub Actions detected, setting VCPKG_ROOT to ${VCPKG_ROOT}")
        elseif(EXISTS "${CMAKE_SOURCE_DIR}/vcpkg")
            set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/vcpkg")
            message(STATUS "Setting VCPKG_ROOT to ${CMAKE_SOURCE_DIR}/vcpkg")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")
            set(VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")
            message(STATUS "Setting VCPKG_ROOT to ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")
        else()
            message(WARNING 
                "VCPKG_ROOT environment variable not set and vcpkg directory not found.\n"
                "Expected locations checked:\n"
                "  $ENV{VCPKG_ROOT}\n"
                "  ${CMAKE_SOURCE_DIR}/vcpkg\n"
                "  ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg\n"
                "Will attempt to proceed with package detection anyway."
            )
            set(VCPKG_ROOT "${CMAKE_BINARY_DIR}/vcpkg")
        endif()
        
        # Set environment variable for other CMake scripts
        set(ENV{VCPKG_ROOT} "${VCPKG_ROOT}")
        message(STATUS "Final VCPKG_ROOT path: ${VCPKG_ROOT}")
    endif()
endif()

# Set Boost versions from parent scope if not already set
if(NOT DEFINED OPENMIND_PREFERRED_BOOST_VERSION)
    set(OPENMIND_PREFERRED_BOOST_VERSION 1.83.0)
endif()
if(NOT DEFINED OPENMIND_REQUIRED_BOOST_VERSION)
    set(OPENMIND_REQUIRED_BOOST_VERSION 1.81.0)
endif()

# Set additional search paths for Windows
if(WIN32)
    # Check standard install locations first
    set(_BOOST_ROOT "")
    if(DEFINED ENV{BOOST_ROOT})
        set(_BOOST_ROOT $ENV{BOOST_ROOT})
    else()
        foreach(_DIR
            "C:/local/boost"
            "C:/local/boost_${OPENMIND_PREFERRED_BOOST_VERSION}"
            "C:/Boost"
            "C:/Program Files/Boost"
            "C:/Program Files (x86)/Boost"
            "$ENV{ProgramFiles}/Boost"
            "$ENV{ProgramFiles\(x86\)}/Boost"
        )
            if(EXISTS "${_DIR}")
                set(_BOOST_ROOT "${_DIR}")
                break()
            endif()
        endforeach()
    endif()

    if(_BOOST_ROOT)
        set(BOOST_ROOT "${_BOOST_ROOT}" CACHE PATH "Boost root directory")
        set(Boost_NO_SYSTEM_PATHS ON)
    endif()

    # Set library directory if specified
    if(DEFINED ENV{BOOST_LIBRARYDIR})
        set(BOOST_LIBRARYDIR $ENV{BOOST_LIBRARYDIR})
    endif()
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_DEBUG ON)  # Enable debug output

# Define required components once to avoid repetition
set(REQUIRED_BOOST_COMPONENTS
    unit_test_framework
    filesystem
    system
    thread
    serialization
)

# Try to find Boost only once
if(NOT Boost_FOUND)
    # Try preferred version first with CONFIG
    find_package(Boost ${OPENMIND_PREFERRED_BOOST_VERSION}
        COMPONENTS ${REQUIRED_BOOST_COMPONENTS}
        CONFIG
        QUIET
    )

    if(NOT Boost_FOUND)
        message(STATUS "Trying fallback to non-CONFIG mode with required version ${OPENMIND_REQUIRED_BOOST_VERSION}")
        # Fall back to regular find_package without CONFIG
        find_package(Boost ${OPENMIND_REQUIRED_BOOST_VERSION}
            COMPONENTS ${REQUIRED_BOOST_COMPONENTS}
        )
    endif()
endif()

if(Boost_FOUND)
    message(STATUS "Found Boost version: ${Boost_VERSION}")
    message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost library dirs: ${Boost_LIBRARY_DIRS}")
    
    # Ensure targets are created for older CMake versions
    if(NOT TARGET Boost::headers)
        add_library(Boost::headers INTERFACE IMPORTED)
        set_target_properties(Boost::headers PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
        )
    endif()
    
    # Create imported targets for each component
    foreach(component ${REQUIRED_BOOST_COMPONENTS})
        string(TOUPPER ${component} UPPER_COMPONENT)
        if(NOT TARGET Boost::${component} AND Boost_${UPPER_COMPONENT}_FOUND)
            add_library(Boost::${component} SHARED IMPORTED GLOBAL)
            set_target_properties(Boost::${component} PROPERTIES
                IMPORTED_LOCATION "${Boost_${UPPER_COMPONENT}_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
            )
        endif()
    endforeach()
else()
    message(STATUS "Could not find Boost")
    message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
    message(STATUS "BOOST_ROOT: ${BOOST_ROOT}")
    message(STATUS "BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}")
    message(STATUS "BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}")
endif()

# Set Boost_DIR to prevent repeated find_package calls
if(NOT Boost_DIR)
    set(Boost_DIR "${Boost_INCLUDE_DIRS}" CACHE PATH "Boost directory" FORCE)
endif()

find_package_handle_standard_args(Boost
    REQUIRED_VARS 
        Boost_FOUND
        Boost_INCLUDE_DIRS
    VERSION_VAR Boost_VERSION
)
