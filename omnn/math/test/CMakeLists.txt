# Create test targets for math module
add_executable(Exponentiation_test Exponentiation_test.cpp)
target_link_libraries(Exponentiation_test PRIVATE math Boost::unit_test_framework)
add_test(NAME Exponentiation_test COMMAND Exponentiation_test)
