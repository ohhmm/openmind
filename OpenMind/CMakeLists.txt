cmake_minimum_required(VERSION 3.14)

include(bins)

get_target_name(this_target)
PROJECT(${this_target})

file(GLOB ${this_target}_SOURCE_FILES *.cpp)

SET(${this_target}_HEADER_FILES
    ArithmeticFacility.h
	ConsoleGeneratorProxy.h
	Facility.h
	FunctorFacility.h
	GeneralGoalGenerator.h
	GeneratorProxy.h
	Goal.h
	GoalGenerator.h
	GoalGeneratorBase.h
	HierarchicalKnowledgeMatrix.h
	IdleTimeGoalGenerator.h
    LogicOffloadFacility.h
    Mind.h
	Named.h
	ReachGoal.h
	RunProcess.h
	SingletonGoalGenerator.h
	Solver.h
	stdafx.h
	targetver.h
	trie.h
	WaitForComputerIdle.h
    )

SOURCE_GROUP("Header Files" FILES 
	targetver.h
	stdafx.h
	Resource.h
	Mind.h
	)
SOURCE_GROUP("Util\\GeneralClasses" FILES 
	Named.cpp
	Named.h
	)
SOURCE_GROUP("Util\\Sync" FILES 
	
	)
SOURCE_GROUP("Generic" FILES 
	Context.cpp
	Context.h
	trie.cpp
	trie.h
	)
SOURCE_GROUP("Util" FILES 
	
	)
SOURCE_GROUP("Util\\HelperFunctors" FILES 
	RiseEventFn.h
	RiseEventFn.cpp
	)
SOURCE_GROUP("Goals\\GoalGenerators\\GeneratorProxy" FILES 
	ConsoleGeneratorProxy.cpp
	ConsoleGeneratorProxy.h
	GeneratorProxy.cpp
	GeneratorProxy.h
	)
SOURCE_GROUP("Source Files" FILES 
	Mind.cpp
	OpenMind.cpp
	stdafx.cpp
	)
SOURCE_GROUP("Solvers" FILES 
	Solver.h
	Solver.cpp
	)
SOURCE_GROUP("Goals" FILES 
	Goal.cpp
	Goal.h
	WaitForComputerIdle.cpp
	WaitForComputerIdle.h
	)
SOURCE_GROUP("Facilities" FILES
	ArithmeticFacility.cpp
	ArithmeticFacility.h
	Facility.cpp
	Facility.h
	FunctorFacility.cpp
	FunctorFacility.h
	HierarchicalKnowledgeMatrix.cpp
	HierarchicalKnowledgeMatrix.h
	LogicOffloadFacility.cpp
	LogicOffloadFacility.h
	ReachGoal.cpp
	ReachGoal.h
	RunProcess.cpp
	RunProcess.h
	)
SOURCE_GROUP("Goals\\GoalGenerators" FILES 
	GeneralGoalGenerator.h
	GoalGenerator.cpp
	GoalGenerator.h
	GoalGeneratorBase.h
	IdleTimeGoalGenerator.cpp
	IdleTimeGoalGenerator.h
	SingletonGoalGenerator.h
	)
SOURCE_GROUP("Solvers\\Generic" FILES
	)

## section: precompiled header	
IF(MSVC)
    ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" MySources)
ENDIF(MSVC)
#ADD_LIBRARY(MyLibrary ${MySources})

SET_SOURCE_FILES_PROPERTIES(${this_target}_HEADER_FILES
                            PROPERTIES HEADER_FILE_ONLY ON)
LIST(APPEND ${this_target}_SOURCE_FILES ${${this_target}_HEADER_FILES})

IF(NOT MSVC)
	ADD_DEFINITIONS(-pthread)
ENDIF(NOT MSVC)

add_library(${this_target} ${${this_target}_SOURCE_FILES})
file(GLOB headers *.h *.hpp *.inc)
APPLY_TARGET_COMMONS(${this_target})
set_target_properties(${this_target} PROPERTIES
	CXX_STANDARD ${CMAKE_CXX_STANDARD}
	FOLDER "libs"
	PUBLIC_HEADER "${headers}"
	)

if(UNIX)
	dep(X11::Xss)
endif(UNIX)

target_compile_definitions(${this_target} PUBLIC
	UNICODE
	_UNICODE
	)

target_include_directories(${this_target} PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/include
    )

target_link_libraries(${this_target} PUBLIC math)

install(TARGETS ${this_target})
