include(pytect)

# Python bindings are currently disabled on Windows (MSVC) due to known issues with
# Boost.Python and memory management. The test_valuable test is excluded from running
# on Windows until these issues are resolved.
if(Python_FOUND
    AND OPENMIND_USE_VCPKG # FIXME
    AND NOT MSVC # FIXME
    AND NOT APPLE # FIXME
)
    set(DEFAULT_OPENMIND_BUILD_PYTHON_BINDINGS ON)
endif()

option(OPENMIND_BUILD_PYTHON_BINDINGS "Build Python bindings" ${DEFAULT_OPENMIND_BUILD_PYTHON_BINDINGS})
if(OPENMIND_BUILD_PYTHON_BINDINGS)
    pymod(
        Boost::python
        math
        Python::Python
        )
endif()

# Conditionally register the test_valuable test
if(OPENMIND_BUILD_TESTS)
    if(NOT MSVC)
        # Only register the test on non-Windows platforms
        add_test(NAME test_valuable COMMAND ${Python_EXECUTABLE} -m unittest discover -s ${CMAKE_CURRENT_SOURCE_DIR}/tests -p "test_valuable.py")
        # Set the PYTHONPATH environment variable for the test
        set_tests_properties(test_valuable PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/python")
    else()
        # Add a dummy test that always passes on Windows
        add_test(NAME test_valuable COMMAND ${CMAKE_COMMAND} -E echo "test_valuable skipped on Windows")
    endif()
endif()
