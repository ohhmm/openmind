# OpenMind Neural Networks
SET (this_target omnn)
project(${this_target})
file(GLOB src *.cpp)
add_library(${this_target} ${src})
#include_directories(${OpenMind_Incs})
#target_link_libraries(${this_target} OpenMind ${OpenMind_Libs})