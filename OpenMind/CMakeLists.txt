cmake_minimum_required(VERSION 3.6)

SET (this_target OpenMind)
PROJECT(${this_target})


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

#FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    ADD_DEFINITIONS( "-DHAS_BOOST" )
    SET(Boost_USE_STATIC_LIBS ON)
ENDIF()

add_definitions(-DUNICODE -D_UNICODE)

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

## section: add definitions
#   add prefix -D. example> -DSHP
#  - DO NOT add  the following definitions(already defined in ${OSP_DEFINITIONS}:
#   -DSHP, -DWIN32, -D_WINDOWS, -D_DEBUG, -D_USRDLL, -D_CRT_SECURE_NO_DEPRECATE
IF(NOT MSVC)
	ADD_DEFINITIONS(-pthread)
ENDIF(NOT MSVC)
#    -DWIN32
#	-D_DEBUG
#	-D_WINDOWS
#    )

## section: add target


add_library(${this_target} ${${this_target}_SOURCE_FILES})
#ADD_EXECUTABLE(${this_target} ${${this_target}_SOURCE_FILES})

if(UNIX)
	find_package(X11)
	if(X11_FOUND)
		set(${this_target}_Incs ${X11_X11_INCLUDE_PATH} ${X11_Xscreensaver_INCLUDE_PATH}
				CACHE INTERNAL "${this_target}: Include Directories" FORCE)
		target_include_directories(${this_target} PUBLIC
			${${this_target}_Incs})

		set(${this_target}_Libs ${X11_X11_LIB} ${X11_Xscreensaver_LIB} pthread
				CACHE INTERNAL "${this_target}: Libraries" FORCE)
                target_link_libraries(${this_target} PUBLIC ${${this_target}_Libs})
		#target_link_libraries(${this_target} lib${this_target})
	endif()
endif(UNIX)


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

target_link_directories(${this_target} PUBLIC ${CMAKE_BINARY_DIR}/lib)

target_link_libraries(${this_target} PUBLIC math)




