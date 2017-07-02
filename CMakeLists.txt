cmake_minimum_required(VERSION 3.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(_IS_DEBUG TRUE)
else()
	set(_IS_DEBUG FALSE)
endif()

option(OPENMIND_BUILD_SAMPLES "Build various samples" ${_IS_DEBUG})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

libfind_package(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ELSEIF(WIN32)
    find_program(NUGET nuget HINTS $ENV{ProgramFiles})
    if(NOT NUGET)
        message(FATAL "Cannot find nuget command line tool.\nInstall it with e.g. choco install nuget.commandline")
    else()
        execute_process(COMMAND ${NUGET} install boost)
    endif()
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
