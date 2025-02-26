
if(NOT Python_EXECUTABLE)
    find_package(Python COMPONENTS Interpreter Development Development.Module Development.Embed)
    if(NOT Python_FOUND AND WIN32 AND EXISTS "$ENV{APPDATA}/Python")
        find_package(Python HINTS "$ENV{APPDATA}/Python/*/")
    endif()
endif()

if(Python_FOUND)
    message(STATUS "Python found: ${Python_EXECUTABLE}")
    message(STATUS "Python version: ${Python_VERSION}")
    set(PYTHON_DESTINATION_VERSION "${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}")
endif()

macro(pymod)
    if(NOT PYTHON_DESTINATION_VERSION)
        set(PYTHON_DESTINATION_VERSION 3)
    else()
        find_package(Boost COMPONENTS python${Python_VERSION_MAJOR}${Python_VERSION_MINOR} REQUIRED)
    endif()
    set(LIBRARY_DESTINATION "lib/python${PYTHON_DESTINATION_VERSION}/site-packages")
    mod(${ARGN})
    set_target_properties(${this_target} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python"
    )
    if(WIN32)
        set_target_properties(${this_target} PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
            SUFFIX ".pyd"
        )
    endif()
endmacro()
