test()

# Define test executables
add_executable(test_08_System 08_System.cpp)

# Link test executables with necessary libraries
target_link_libraries(test_08_System PUBLIC Boost::thread Valuable)

# Set output directory for test executables
set_target_properties(test_08_System PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
