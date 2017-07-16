set(this_target opengl_rt_laby)
project(${this_target})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

file(GLOB src *.cpp *.h)
add_executable(${this_target} ${src})
include_directories(${OpenMind_Incs})
target_link_libraries(${this_target} TrainedUnit omnn)
