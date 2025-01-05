cmake_minimum_required(VERSION 3.14)
project(debug_test CXX)

set(CMAKE_CXX_STANDARD 23)
add_compile_definitions(
    BOOST_TEST_NO_LIB
    BOOST_ALL_NO_LIB
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(minimal_test src/test.cpp)
target_link_libraries(minimal_test PRIVATE Boost::unit_test_framework)

# Set output directory to match other tests
set_target_properties(minimal_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
)

# Ensure test source directory exists and is in include path
target_include_directories(minimal_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
