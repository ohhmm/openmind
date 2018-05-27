get_filename_component(this_target ${CMAKE_CURRENT_SOURCE_DIR} NAME)

project(${this_target})
file(GLOB src *.cpp *.h)
add_library(${this_target} ${src})
target_link_libraries(${this_target} boost_serialization)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (${OPENMIND_BUILD_TESTS})
    add_subdirectory(test)
endif ()

if (${OPENMIND_BUILD_3RD_PARTY_TESTS})
    add_subdirectory(3rdPartyTests)
endif ()
