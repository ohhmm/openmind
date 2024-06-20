test()

# Define test executables
add_executable(test_08_System 08_System.cpp)
add_executable(test_fastest_get fastest_get.cpp)

# Link test executables with necessary libraries
target_link_libraries(test_08_System PUBLIC Boost::thread Valuable)
target_link_libraries(test_fastest_get PUBLIC Boost::thread Valuable)

# Set output directory for test executables
set_target_properties(test_08_System PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(test_fastest_get PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
