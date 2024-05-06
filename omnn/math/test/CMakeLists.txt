# Define the test executable for the math tests
add_executable(math_tests
    SchoolTasks.cpp
    # Add other test source files here
)

# Include the directory where Sum.h and other headers are located
target_include_directories(math_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_include_directories(math_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../)
# Include the directory where tasq.h and other headers are located
target_include_directories(math_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../rt)
# Include the directory where Variable.h and other headers are located
target_include_directories(math_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../)

# Link the test executable with the necessary libraries
target_link_libraries(math_tests
    PUBLIC
    math # Link against the math library
    rt
    Boost::thread
    Boost::unit_test_framework
)

# Ensure the rt library is built before the math_tests executable
add_dependencies(math_tests rt)

# Define the test cases for CTest
add_test(NAME MathTests COMMAND math_tests)
