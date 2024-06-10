# Define the test target for Polyfit_test
add_executable(Polyfit_test Polyfit_test.cpp)
target_link_libraries(Polyfit_test PUBLIC math_lib Boost::thread)
target_include_directories(Polyfit_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../rt)

# Define the test target for VarHost_test
add_executable(VarHost_test VarHost_test.cpp)
target_link_libraries(VarHost_test PUBLIC math_lib Boost::thread)
target_include_directories(VarHost_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../rt)

# Define the test target for Modulo_test
add_executable(Modulo_test Modulo_test.cpp)
target_link_libraries(Modulo_test PUBLIC math_lib Boost::thread)
target_include_directories(Modulo_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../rt)

# Define the test target for Sum_test
add_executable(Sum_test Sum_test.cpp)
target_link_libraries(Sum_test PUBLIC math_lib Boost::thread)
target_include_directories(Sum_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../rt)

# Define the test target for FANN_test
add_executable(FANN_test ../3rdPartyTests/FANN_test.cpp)
target_link_libraries(FANN_test PUBLIC math_lib Boost::thread fann)
target_include_directories(FANN_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../rt)

# Add tests to CTest
add_test(NAME Polyfit_test COMMAND Polyfit_test)
add_test(NAME VarHost_test COMMAND VarHost_test)
add_test(NAME Modulo_test COMMAND Modulo_test)
add_test(NAME Sum_test COMMAND Sum_test)
add_test(NAME FANN_test COMMAND FANN_test)
