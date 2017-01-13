cmake_minimum_required(VERSION 3.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(_IS_DEBUG TRUE)
else()
	set(_IS_DEBUG FALSE)
endif()

option(OPENMIND_BUILD_SAMPLES "Build various samples" ${_IS_DEBUG})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

IF(NOT MSVC)
    ADD_DEFINITIONS(-std=c++14)
ENDIF(NOT MSVC)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(OpenMind)
add_subdirectory(omnn)
if(${OPENMIND_BUILD_SAMPLES})
	add_subdirectory(Examples/ChatBot)
	add_subdirectory(Examples/IdleTimeWorker)
	add_subdirectory(Examples/Arithmetics)
    add_subdirectory(Examples/Mnist)
ENDIF()
