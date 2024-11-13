enable_testing()

# Add the test executables
add_executable(Less_comparator_test Less_comparator_test.cpp)
add_executable(IsNegativeThan_comparator_test IsNegativeThan_comparator_test.cpp)

# Link against required libraries
target_link_libraries(Less_comparator_test PRIVATE
    logic
    math
    Boost::unit_test_framework
)

target_link_libraries(IsNegativeThan_comparator_test PRIVATE
    logic
    math
    Boost::unit_test_framework
)

# Add the tests to CTest
add_test(NAME Less_comparator_test COMMAND Less_comparator_test)
add_test(NAME IsNegativeThan_comparator_test COMMAND IsNegativeThan_comparator_test)
