test()
#target_link_libraries(08_System PUBLIC Boost::thread)

# Add the new test executable for angle solvability
add_executable(angle_solvability_test_unique angle_solvability_test.cpp)

# Link the test executable with necessary libraries
target_link_libraries(angle_solvability_test_unique PUBLIC OpenMind::math)

# Add the test to CTest
add_test(NAME AngleSolvabilityTest COMMAND angle_solvability_test_unique)
