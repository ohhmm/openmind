
if(NOT Python_EXECUTABLE)
    find_package(Python)
    if(NOT Python_FOUND AND WIN32)
        find_package(Python PATHS "$ENV{APPDATA}/Python/*/")
    endif()
endif()

if(Python_FOUND)
    message(STATUS "Python found: ${Python_EXECUTABLE}")
    message(STATUS "Python version: ${Python_VERSION}")
endif()
