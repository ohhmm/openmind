include(pytect)

if(Python_FOUND
    AND OPENMIND_USE_VCPKG # FIXME
    AND NOT MSVC # FIXME
    AND NOT APPLE # FIXME
)
    set(DEFAULT_OPENMIND_BUILD_PYTHON_BINDINGS ON)
endif()

option(OPENMIND_BUILD_PYTHON_BINDINGS "Build Python bindings" ${DEFAULT_OPENMIND_BUILD_PYTHON_BINDINGS})
if(OPENMIND_BUILD_PYTHON_BINDINGS)
    pymod(
        Boost::python
        math
        Python::Python
        )
endif()