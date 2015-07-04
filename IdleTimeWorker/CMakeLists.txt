project(IdleTimeWorker)
file(GLOB src *.cpp)
add_executable(IdleTimeWorker ${src})
target_link_libraries(IdleTimeWorker libOpenMind)