enable_testing()

# Define test source directory macro
get_filename_component(TEST_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
add_definitions(-DTEST_SRC_DIR="${TEST_SRC_DIR}/")

# Add definition to disable OpenCL requirement in tests
add_definitions(-DDISABLE_OPENCL_TESTS)

# Configure Boost for testing
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)

if(APPLE)
    set(CMAKE_FIND_FRAMEWORK LAST)
    set(CMAKE_FIND_APPBUNDLE LAST)
endif()

# Find required packages with specific version
find_package(Boost ${OPENMIND_REQUIRED_BOOST_VERSION} REQUIRED 
    COMPONENTS 
        unit_test_framework 
        filesystem 
        system 
        thread
        serialization
)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Please install Boost development libraries.")
endif()

# Add Boost include directories
# Using static linking, so we don't need dynamic link definitions

if(DEFINED ENV{CI})
    # Use parent scope variables
    include_directories(SYSTEM 
        "${VCPKG_INCLUDE_DIR}"
        "${Boost_INCLUDE_DIRS}"
    )
    
    if(EXISTS "${VCPKG_LIB_DIR}")
        link_directories("${VCPKG_LIB_DIR}")
    else()
        message(WARNING "VCPKG library directory not found: ${VCPKG_LIB_DIR}")
    endif()
else()
    include_directories(SYSTEM "${Boost_INCLUDE_DIRS}")
endif()

# Add debug output
message(STATUS "Test include directories:")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "  ${dir}")
endforeach()

# Link against Boost libraries
# Use modern CMake target-based approach for Boost
if(NOT TARGET Boost::unit_test_framework)
    add_library(Boost::unit_test_framework INTERFACE IMPORTED GLOBAL)
    set_target_properties(Boost::unit_test_framework PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}"
    )
endif()

if(NOT TARGET Boost::filesystem)
    add_library(Boost::filesystem INTERFACE IMPORTED GLOBAL)
    set_target_properties(Boost::filesystem PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${Boost_FILESYSTEM_LIBRARY}"
    )
endif()

if(NOT TARGET Boost::system)
    add_library(Boost::system INTERFACE IMPORTED GLOBAL)
    set_target_properties(Boost::system PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${Boost_SYSTEM_LIBRARY}"
    )
endif()

if(NOT TARGET Boost::thread)
    add_library(Boost::thread INTERFACE IMPORTED GLOBAL)
    set_target_properties(Boost::thread PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${Boost_THREAD_LIBRARY}"
    )
endif()

if(NOT TARGET Boost::serialization)
    add_library(Boost::serialization INTERFACE IMPORTED GLOBAL)
    set_target_properties(Boost::serialization PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${Boost_SERIALIZATION_LIBRARY}"
    )
endif()

# Ensure TBB is properly linked
if(TARGET TBB::tbb)
    link_libraries(TBB::tbb)
endif()

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Please install Boost development libraries.")
endif()

# Ensure all Boost targets exist
if(NOT TARGET Boost::serialization)
    add_library(Boost::serialization INTERFACE IMPORTED)
    set_target_properties(Boost::serialization PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${Boost_SERIALIZATION_LIBRARY}"
    )
endif()

# Find all test source files
file(GLOB TEST_SOURCES "*.cpp")

# Create test targets for each source file
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    # Core dependencies
    target_link_libraries(${TEST_NAME} 
        PRIVATE 
            math
            storage
            Boost::unit_test_framework
            Boost::filesystem
            Boost::system
            Boost::thread
            Boost::serialization
    )

    # Add TBB dependency if available
    if(TARGET TBB::tbb)
        target_link_libraries(${TEST_NAME} PRIVATE 
            TBB::tbb
            $<$<BOOL:${WIN32}>:${CMAKE_DL_LIBS}>
        )
        target_compile_definitions(${TEST_NAME} PRIVATE OPENMIND_USE_TBB)
        
        # Ensure TBB is linked after Boost
        set_property(TARGET ${TEST_NAME} PROPERTY LINK_INTERFACE_MULTIPLICITY 3)
        if(MSVC)
            set_target_properties(${TEST_NAME} PROPERTIES LINK_FLAGS_RELEASE "/OPT:REF")
        endif()
        
        # Add runtime path for TBB on Unix platforms
        if(UNIX AND NOT APPLE)
            set_target_properties(${TEST_NAME} PROPERTIES
                BUILD_WITH_INSTALL_RPATH TRUE
                INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN"
            )
        elseif(APPLE)
            set_target_properties(${TEST_NAME} PROPERTIES
                BUILD_WITH_INSTALL_RPATH TRUE
                INSTALL_RPATH "@loader_path/../lib;@loader_path"
            )
        endif()
    endif()

    # Platform-specific dependencies
    if(NOT WIN32)
        if(NOT APPLE)
            find_library(RT_LIBRARY rt)
            if(RT_LIBRARY)
                target_link_libraries(${TEST_NAME} PRIVATE ${RT_LIBRARY})
            endif()
        endif()
        find_package(Threads REQUIRED)
        target_link_libraries(${TEST_NAME} PRIVATE Threads::Threads)
    endif()

    # Set test properties
    set_target_properties(${TEST_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    
    # Add Boost Test definitions
    target_compile_definitions(${TEST_NAME} PRIVATE
        BOOST_TEST_MAIN
    )

    # Register with CTest
    add_test(NAME ${TEST_NAME} 
             COMMAND ${CMAKE_BINARY_DIR}/bin/${TEST_NAME}
             WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    
    # Set test dependencies
    add_dependencies(${TEST_NAME} math storage)

    # Set environment to indicate OpenCL tests should be skipped
    set_tests_properties(${TEST_NAME} PROPERTIES
        ENVIRONMENT "SKIP_OPENCL_TESTS=1;LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib")
endforeach()

# Print Boost configuration for debugging
message(STATUS "Boost_VERSION: ${Boost_VERSION}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_SERIALIZATION_LIBRARY: ${Boost_SERIALIZATION_LIBRARY}")
