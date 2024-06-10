if(Boost_FOUND)
    if(NOT MSVC OR OPENMIND_USE_CONAN)
        set(DEPENDENCIES Boost::filesystem)
        if(OPENMIND_BUILD_TESTS)
            list(APPEND DEPENDENCIES
                Boost::test_exec_monitor
                Boost::unit_test_framework
            )
        endif()
    endif()
endif()

if(OPENMIND_USE_OPENCL)
    list(APPEND DEPENDENCIES OpenCL)
endif()

add_library(rt
    GC.cpp
    Neuron.cpp
    Prime.cpp
    cl.cpp
    tasq.cpp
)

target_link_libraries(rt PUBLIC ${DEPENDENCIES} math_lib storage)

# Add include directories for header files
target_include_directories(rt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
