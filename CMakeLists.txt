cmake_minimum_required(VERSION 3.0)
set(PRODUCT_NAME "OpenMind")
set (CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(_IS_DEBUG TRUE)
else()
	set(_IS_DEBUG FALSE)
endif()

option(OPENMIND_BUILD_SAMPLES "Build various samples" ${_IS_DEBUG})
option(OPENMIND_BUILD_TESTS "Build various TESTS" ${_IS_DEBUG})
option(OPENMIND_BUILD_3RD_PARTY_TESTS "Build various 3rd party TESTS to compare to openmind by performance" FALSE)

include_directories(${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/boost/gil/include
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
find_package(Boost)
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

if(MSVC)
	if(Boost_FOUND)
		link_directories(${Boost_INCLUDE_DIR}/stage/lib)
	endif()
endif()

find_package(opencl)


if (${OPENMIND_BUILD_TESTS})
    enable_testing()
endif ()

add_subdirectory(OpenMind)
add_subdirectory(omnn)
if(${OPENMIND_BUILD_SAMPLES})
	add_subdirectory(Examples/ChatBot)
	add_subdirectory(Examples/IdleTimeWorker)
	add_subdirectory(Examples/Arithmetics)
    add_subdirectory(Examples/Mnist)
    add_subdirectory(Examples/opengl-rt-laby)
ENDIF()

