cmake_minimum_required(VERSION 3.14)
project(debug_test CXX)

set(CMAKE_CXX_STANDARD 23)
add_compile_definitions(
    BOOST_TEST_NO_LIB
    BOOST_ALL_NO_LIB
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(minimal_test src/test.cpp)
target_link_libraries(minimal_test PRIVATE Boost::unit_test_framework)
