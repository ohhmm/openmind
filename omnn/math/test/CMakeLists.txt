SET (lib_target ${this_target})
SET (this_target test_${lib_target})
project(${this_target})
file(GLOB src *.cpp)
add_definitions(
    -DTEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
    -DTEST_BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}/"
    )
link_directories(
    ${Boost_LIBRARIES}
    /usr/local/lib
    ${Boost_INCLUDE_DIR}/stage/lib
    )
if(NOT MSVC)
    set(test_libs boost_test_exec_monitor)
endif()
foreach(TEST ${src})
    get_filename_component(TEST_NAME ${TEST} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_NAME})
    target_link_libraries(${TEST_NAME} extrapolator ${test_libs})
    add_test(NAME ${TEST_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME})
endforeach()
