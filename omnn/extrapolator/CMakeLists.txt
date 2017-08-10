SET (this_target extrapolator)
project(${this_target})
file(GLOB src *.cpp *.h)
add_library(${this_target} ${src})

if (${OPENMIND_BUILD_TESTS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(test)
endif ()