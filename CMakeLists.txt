cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

IF(NOT MSVC)
    ADD_DEFINITIONS(-std=c++11)
ENDIF(NOT MSVC)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(OpenMind)
add_subdirectory(ChatBot)
add_subdirectory(IdleTimeWorker)
add_subdirectory(Arithmetics)
