<<<<<<< HEAD
set(DEPENDENCIES boost)
if(Boost_FOUND)
    if(NOT MSVC OR OPENMIND_USE_CONAN)
        set(DEPENDENCIES ${DEPENDENCIES} Boost::filesystem)
        if(OPENMIND_BUILD_TESTS)
            set(DEPENDENCIES ${DEPENDENCIES}
                Boost::test_exec_monitor
                Boost::unit_test_framework
                )
        endif()
    endif()
endif()

if(OPENMIND_USE_OPENCL)
    set(DEPENDENCIES ${DEPENDENCIES} OpenCL)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

lib(${DEPENDENCIES})
||||||| 9cf9038a3
# OpenMind Neural Networks
get_filename_component(this_target ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${this_target})
file(GLOB src *.cpp *.h)
add_library(omnn ${src})
#include_directories(${OpenMind_Incs})
#target_link_libraries(${this_target} OpenMind ${OpenMind_Libs})
=======

set(DEPENDENCIES boost)
if(Boost_FOUND)
    if(NOT MSVC OR OPENMIND_USE_CONAN)
        set(DEPENDENCIES ${DEPENDENCIES} Boost::filesystem)
        if(OPENMIND_BUILD_TESTS)
            set(DEPENDENCIES ${DEPENDENCIES}
                Boost::test_exec_monitor
                Boost::unit_test_framework
                )
        endif()
    endif()
endif()

if(OPENMIND_USE_OPENCL)
	set(DEPENDENCIES ${DEPENDENCIES} OpenCL)
endif()

lib(${DEPENDENCIES})
>>>>>>> origin/onnx-integration
