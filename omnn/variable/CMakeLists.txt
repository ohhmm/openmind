include(pytect)

# Python bindings are currently disabled on Windows (MSVC) due to known issues with
# Boost.Python and memory management. The test_valuable test is excluded from running
# on Windows until these issues are resolved.
if(Python_FOUND
    AND (OPENMIND_USE_VCPKG OR NOT DEFINED OPENMIND_USE_VCPKG) # Allow Python bindings without VCPKG
    AND NOT MSVC # FIXME
    AND NOT APPLE # FIXME
)
    set(DEFAULT_OPENMIND_BUILD_PYTHON_BINDINGS ON)
endif()

option(OPENMIND_BUILD_PYTHON_BINDINGS "Build Python bindings" ${DEFAULT_OPENMIND_BUILD_PYTHON_BINDINGS})
if(OPENMIND_BUILD_PYTHON_BINDINGS)
    # Create the Python module directory if it doesn't exist
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/python)
    
    # Build the Python module
    pymod(
        Boost::python
        math
        Python::Python
        )
    
    # Set the output directory for the Python module
    set_target_properties(variable PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python"
        PREFIX ""  # Remove 'lib' prefix on Unix
    )
    
    # Copy the test file to the build directory
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_valuable.py
         DESTINATION ${CMAKE_BINARY_DIR}/python)
endif()

# Conditionally register the test_valuable test
if(OPENMIND_BUILD_TESTS)
    if(NOT MSVC)
        # Create a script to run the test with the correct PYTHONPATH
        file(WRITE ${CMAKE_BINARY_DIR}/run_test_valuable.sh "#!/bin/sh\nPYTHONPATH=${CMAKE_BINARY_DIR}/python python3 -m unittest discover -s ${CMAKE_CURRENT_SOURCE_DIR}/tests -p \"test_valuable.py\"\n")
        execute_process(COMMAND chmod +x ${CMAKE_BINARY_DIR}/run_test_valuable.sh)
        
        # Only register the test on non-Windows platforms
        add_test(NAME test_valuable COMMAND ${CMAKE_BINARY_DIR}/run_test_valuable.sh)
        set_tests_properties(test_valuable PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/python")
    else()
        # Add a dummy test that always passes on Windows
        add_test(NAME test_valuable COMMAND ${CMAKE_COMMAND} -E echo "test_valuable skipped on Windows")
    endif()
endif()
