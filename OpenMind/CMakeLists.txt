cmake_minimum_required(VERSION 2.8)

## section: Macro
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledBinary}\" /FI\"${PrecompiledBinary}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

## start setting
SET (this_target OpenMind)
PROJECT(${this_target})

FIND_PACKAGE(Boost REQUIRED)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
    SET(Boost_USE_STATIC_LIBS ON)
ENDIF()
    


add_definitions(-DUNICODE -D_UNICODE)


## section: include directory
INCLUDE_DIRECTORIES(
  
  )

## section: source files
# Add your source files here (one file per line), please SORT in alphabetical order for future maintenance
SET (${this_target}_SOURCE_FILES
    Mind.cpp
	OpenMind.cpp
	stdafx.cpp
	Facility.cpp
	FunctorFacility.cpp
	ReachGoal.cpp
	Goal.cpp
	SolveProportion.cpp
	WaitForComputerIdle.cpp
	GoalGenerator.cpp
	IdleTimeGoalGenerator.cpp
	ProportionGenerator.cpp
	ConsoleGeneratorProxy.cpp
	GeneratorProxy.cpp
	GetEquation.cpp
	SolveEquation.cpp
	Named.cpp
	RunProcess.cpp
    )

if(MSVC)
	list(APPEND ${this_target}_SOURCE_FILES RunProcess.cpp)
endif(MSVC)

## section: header files
# Add your header files here(one file per line), please SORT in alphabetical order for future maintenance!
SET(${this_target}_HEADER_FILES
    Mind.h
	OpenMind.h
	Resource.h
	stdafx.h
	targetver.h
	Facility.h
	FunctorFacility.h
	ReachGoal.h
	RunProcess.h
	Goal.h
	SolveProportion.h
	WaitForComputerIdle.h
	GeneralGoalGenerator.h
	GoalGenerator.h
	GoalGeneratorBase.h
	IdleTimeGoalGenerator.h
	ProportionGenerator.h
	SingletonGoalGenerator.h
	ConsoleGeneratorProxy.h
	GeneratorProxy.h
	GetEquation.h
	SolveEquation.h
	Named.h
    )

SOURCE_GROUP("Header Files" FILES 
	Mind.h
	OpenMind.h
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
	
	)
SOURCE_GROUP("Goals" FILES 
	Goal.cpp
	SolveProportion.cpp
	WaitForComputerIdle.cpp
	Goal.h
	SolveProportion.h
	WaitForComputerIdle.h
	)
SOURCE_GROUP("Resource Files" FILES 
	
	)
SOURCE_GROUP("Facilities" FILES 
	Facility.cpp
	FunctorFacility.cpp
	ReachGoal.cpp
	#RunProcess.cpp
	Facility.h
	FunctorFacility.h
	ReachGoal.h
	RunProcess.h
	)
SOURCE_GROUP("Goals\\GoalGenerators" FILES 
	GoalGenerator.cpp
	IdleTimeGoalGenerator.cpp
	ProportionGenerator.cpp
	GeneralGoalGenerator.h
	GoalGenerator.h
	GoalGeneratorBase.h
	IdleTimeGoalGenerator.h
	ProportionGenerator.h
	SingletonGoalGenerator.h
	)
SOURCE_GROUP("Solvers\\Equation" FILES 
	GetEquation.cpp
	SolveEquation.cpp
	GetEquation.h
	SolveEquation.h
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

## section: add definitions
#   add prefix -D. example> -DSHP
#  - DO NOT add  the following definitions(already defined in ${OSP_DEFINITIONS}:
#   -DSHP, -DWIN32, -D_WINDOWS, -D_DEBUG, -D_USRDLL, -D_CRT_SECURE_NO_DEPRECATE
IF(NOT MSVC)
	ADD_DEFINITIONS(-std=c++11)
ENDIF(MSVC)
#    -DWIN32
#	-D_DEBUG
#	-D_WINDOWS
#    )

## section: add target

ADD_EXECUTABLE(${this_target} ${${this_target}_SOURCE_FILES})

if(linux)
	target_link_libraries(${this_target} X11 Xss)
endif(linux)
