project(Arithmetics)

file(GLOB src *.cpp)

SOURCE_GROUP("Solvers\\Equation" FILES
	../Arithmetics/GetEquation.cpp
	../Arithmetics/SolveEquation.cpp
	../Arithmetics/GetEquation.h
	../Arithmetics/SolveEquation.h
	)

add_executable(arithm ${src})
target_link_libraries(arithm libOpenMind)