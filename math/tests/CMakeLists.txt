# CMake configuration for tests in the math/tests directory

# Add the executable for the sigmoid test
add_executable(test_sigmoid sigmoid.cpp)

# Link the test executable with necessary libraries
target_link_libraries(test_sigmoid PRIVATE ${CMAKE_THREAD_LIBS_INIT})

# Add the test to CTest
add_test(NAME SigmoidTest COMMAND test_sigmoid)

# Add the executable for the ReLU test
add_executable(test_relu relu.cpp)

# Link the test executable with necessary libraries
target_link_libraries(test_relu PRIVATE ${CMAKE_THREAD_LIBS_INIT})

# Add the test to CTest
add_test(NAME ReLUTest COMMAND test_relu)

# Add the executable for the Tanh test
add_executable(test_tanh tanh.cpp)

# Link the test executable with necessary libraries
target_link_libraries(test_tanh PRIVATE ${CMAKE_THREAD_LIBS_INIT})

# Add the test to CTest
add_test(NAME TanhTest COMMAND test_tanh)
