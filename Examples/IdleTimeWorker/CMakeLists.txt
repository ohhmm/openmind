project(IdleTimeWorker)
file(GLOB src *.cpp)

add_executable(IdleTimeWorker ${src})
include_directories(${OpenMind_Incs})
target_link_libraries(IdleTimeWorker OpenMind ${OpenMind_Libs})