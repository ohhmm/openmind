cmake_minimum_required(VERSION 3.0)

# Define the test executable
add_executable(fastest_get fastest_get.cpp)

# Specify the dependencies for the test executable
target_link_libraries(fastest_get
    PRIVATE
        Boost::filesystem
        Boost::system
        Boost::serialization
        Boost::thread
        Boost::test_exec_monitor
        Boost::unit_test_framework
        rt
)

# Include directories for the test executable
target_include_directories(fastest_get
    PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/omnn
)

# Add the test to CTest
add_test(NAME fastest_get COMMAND fastest_get)
