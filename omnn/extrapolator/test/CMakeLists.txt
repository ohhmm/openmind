cmake_minimum_required(VERSION 3.10)

# Get the directory of this CMakeLists.txt file
get_filename_component(TEST_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
get_filename_component(TEST_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}" ABSOLUTE)

# Convert paths to proper format for each platform
file(TO_NATIVE_PATH "${TEST_SRC_DIR}" TEST_SRC_DIR_NATIVE)
file(TO_NATIVE_PATH "${TEST_BIN_DIR}" TEST_BIN_DIR_NATIVE)

# Add definitions that will be available to the compiler
add_definitions(-DTEST_SRC_DIR="${TEST_SRC_DIR_NATIVE}")
add_definitions(-DTEST_BIN_DIR="${TEST_BIN_DIR_NATIVE}")

# Add the test
test()
