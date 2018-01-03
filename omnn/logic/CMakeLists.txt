SET (this_target logic)

project(${this_target})
file(GLOB src *.cpp *.h)
add_library(${this_target} ${src})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (${OPENMIND_BUILD_TESTS})
    add_subdirectory(test)
endif ()
