
if(NOT Python_EXECUTABLE)
    find_package(Python)
    if(NOT Python_FOUND AND WIN32 AND EXISTS "$ENV{APPDATA}/Python")
        find_package(Python HINTS "$ENV{APPDATA}/Python/*/")
    endif()
endif()

if(Python_FOUND)
    message(STATUS "Python found: ${Python_EXECUTABLE}")
    message(STATUS "Python version: ${Python_VERSION}")
endif()
