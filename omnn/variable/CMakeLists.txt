include(pytect)

if(Python_FOUND
    AND OPENMIND_USE_VCPKG # FIXME : remove this line to enable Python bindings for non-VCPKG builds as well
)
    set(DEFAULT_OPENMIND_BUILD_PYTHON_BINDINGS ON)
endif()

option(OPENMIND_BUILD_PYTHON_BINDINGS "Build Python bindings" ${DEFAULT_OPENMIND_BUILD_PYTHON_BINDINGS})
if(OPENMIND_BUILD_PYTHON_BINDINGS)
    pymod(
        Boost::python
        math
        )
endif()