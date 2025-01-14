enable_testing()

# Find required packages
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# Create test executable
add_executable(fastest_get fastest_get.cpp)

# Link against Boost Test and other required libraries
target_link_libraries(fastest_get
    PRIVATE
        rt
        Boost::unit_test_framework
)

# Add test to CTest
add_test(NAME fastest_get COMMAND fastest_get)

# Set test properties
set_target_properties(fastest_get PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add Boost Test definitions
target_compile_definitions(fastest_get PRIVATE
    BOOST_TEST_DYN_LINK
    BOOST_TEST_MAIN
)
