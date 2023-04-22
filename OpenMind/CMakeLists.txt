cmake_minimum_required(VERSION 3.14)

include(bins)
include(pch)

get_target_name(this_target)
PROJECT(${this_target})

IF (NOT Boost_FOUND)
	FIND_PACKAGE(Boost)
ENDIF()

IF (Boost_FOUND)
    ADD_DEFINITIONS( "-DHAS_BOOST" )
    SET(Boost_USE_STATIC_LIBS ON)
ENDIF()

file(GLOB ${this_target}_SOURCE_FILES *.cpp)

if(MSVC)
	list(APPEND ${this_target}_SOURCE_FILES RunProcess.cpp)
endif(MSVC)

## section: header files
# Add your header files here(one file per line), please SORT in alphabetical order for future maintenance!
SET(${this_target}_HEADER_FILES
    Mind.h
    ArithmeticFacility.h
	stdafx.h
	targetver.h
	Facility.h
	FunctorFacility.h
	ReachGoal.h
	RunProcess.h
	Goal.h
	Solver.h
	WaitForComputerIdle.h
	GeneralGoalGenerator.h
	GoalGenerator.h
	GoalGeneratorBase.h
	IdleTimeGoalGenerator.h
	SingletonGoalGenerator.h
	ConsoleGeneratorProxy.h
	GeneratorProxy.h
	Named.h
	trie.h
    )

SOURCE_GROUP("Header Files" FILES 
	Mind.h
	Resource.h
	stdafx.h
	targetver.h
	)
SOURCE_GROUP("Util\\GeneralClasses" FILES 
	Named.cpp
	Named.h
	)
SOURCE_GROUP("Util\\Sync" FILES 
	
	)
SOURCE_GROUP("Generic" FILES 
	trie.cpp
	trie.h
	Context.cpp
	Context.h
	)
SOURCE_GROUP("Util" FILES 
	
	)
SOURCE_GROUP("Util\\HelperFunctors" FILES 
	RiseEventFn.cpp
	RiseEventFn.h
	)
SOURCE_GROUP("Goals\\GoalGenerators\\GeneratorProxy" FILES 
	ConsoleGeneratorProxy.cpp
	GeneratorProxy.cpp
	ConsoleGeneratorProxy.h
	GeneratorProxy.h
	)
SOURCE_GROUP("Source Files" FILES 
	Mind.cpp
	OpenMind.cpp
	stdafx.cpp
	)
SOURCE_GROUP("Solvers" FILES 
	Solver.cpp
	Solver.h
	)
SOURCE_GROUP("Goals" FILES 
	Goal.cpp
	WaitForComputerIdle.cpp
	Goal.h
	WaitForComputerIdle.h
	)
SOURCE_GROUP("Facilities" FILES
	Facility.cpp
	FunctorFacility.cpp
	ReachGoal.cpp
	RunProcess.cpp
	ArithmeticFacility.cpp
	ArithmeticFacility.h
	Facility.h
	FunctorFacility.h
	ReachGoal.h
	RunProcess.h
	)
SOURCE_GROUP("Goals\\GoalGenerators" FILES 
	GoalGenerator.cpp
	IdleTimeGoalGenerator.cpp
	GeneralGoalGenerator.h
	GoalGenerator.h
	GoalGeneratorBase.h
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
                            PROPERTIES HEADER_FILE_ONLY TRUE)
LIST(APPEND ${this_target}_SOURCE_FILES ${${this_target}_HEADER_FILES})

#add_definitions( -DBOOST_ALL_DYN_LINK )
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
if(Boost_INCLUDE_DIR)
	target_include_directories(${this_target} PUBLIC ${Boost_INCLUDE_DIR})
endif()
if(Boost_INCLUDE_DIRS)
	target_include_directories(${this_target} PUBLIC ${Boost_INCLUDE_DIRS})
endif()

target_link_libraries(${this_target} PUBLIC math)




