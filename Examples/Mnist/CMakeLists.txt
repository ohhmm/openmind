set(this_target Mnist)
project(${this_target})
file(GLOB src *.cpp)

add_subdirectory(TrainedUnit)
add_executable(${this_target} ${src})
include_directories(${OpenMind_Incs})
target_link_libraries(${this_target} TrainedUnit omnn)