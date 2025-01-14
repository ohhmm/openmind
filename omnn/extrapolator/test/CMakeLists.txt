enable_testing()

# Find required packages
find_package(Boost REQUIRED COMPONENTS unit_test_framework filesystem system thread serialization)

# Find all test source files
file(GLOB TEST_SOURCES "*.cpp")

# Create test targets for each source file
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    # Core dependencies
    target_link_libraries(${TEST_NAME} 
        PRIVATE 
            extrapolator
            math
            storage
            Boost::unit_test_framework
            Boost::filesystem
            Boost::system
            Boost::thread
            Boost::serialization
    )

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
        BOOST_TEST_DYN_LINK
        BOOST_TEST_MAIN
    )

    # Register with CTest
    add_test(NAME ${TEST_NAME} 
             COMMAND ${CMAKE_BINARY_DIR}/bin/${TEST_NAME}
             WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    
    # Set test dependencies
    add_dependencies(${TEST_NAME} extrapolator math storage)

    # Set environment to indicate OpenCL tests should be skipped
    set_tests_properties(${TEST_NAME} PROPERTIES
        ENVIRONMENT "SKIP_OPENCL_TESTS=1;LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib")
endforeach()
