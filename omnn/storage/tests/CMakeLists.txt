enable_testing()

# Find required packages
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# Create test executable
add_executable(LevelDbStorage LevelDbStorage.cpp)

# Link against Boost Test and other required libraries
target_link_libraries(LevelDbStorage
    PRIVATE
        storage
        Boost::unit_test_framework
)

# Add test to CTest
add_test(NAME LevelDbStorage COMMAND LevelDbStorage)

# Set test properties
set_target_properties(LevelDbStorage PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add Boost Test definitions
target_compile_definitions(LevelDbStorage PRIVATE
    BOOST_TEST_DYN_LINK
    BOOST_TEST_MAIN
)
