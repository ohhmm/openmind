get_filename_component(this_target ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" this_target ${this_target})
set(this_target test_${this_target})
project(${this_target})
file(GLOB src *.cpp)
link_directories(${Boost_LIBRARIES} /usr/local/lib)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(FANN)
include_directories(${FANN_INCLUDE_DIR})

add_definitions(-DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

foreach(TEST ${src})
    get_filename_component(TEST_NAME ${TEST} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_NAME})
    target_link_libraries(${TEST_NAME} extrapolator
            boost_test_exec_monitor-mt
            ${FANN_LIBRARIES}
            )
    add_test(NAME ${TEST_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME})
endforeach()
