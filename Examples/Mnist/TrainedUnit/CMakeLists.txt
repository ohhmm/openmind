set(this_target TrainedUnit)
project(${this_target})
file(GLOB src *.cpp)

add_library(${this_target} ${src})
include_directories(${omnn_Incs})
target_link_libraries(${this_target} omnn)