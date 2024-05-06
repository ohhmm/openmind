cmake_minimum_required(VERSION 3.0)
project(VarHost_test)

# Include the directory where VarHost_test.cpp is located
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Add the test executable
add_executable(VarHost_test VarHost_test.cpp)

# Link with the math library (assuming it's named 'math')
target_link_libraries(VarHost_test math)
