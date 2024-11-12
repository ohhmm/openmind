cmake
if(MSVC)
    # Force consistent runtime library settings first
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

    # Match Boost's iterator debug level for compatibility
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D_ITERATOR_DEBUG_LEVEL=0")
    else()
        add_definitions(-D_ITERATOR_DEBUG_LEVEL=2)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_ITERATOR_DEBUG_LEVEL=2")
    endif()

    # Boost configuration
    set(Boost_USE_DEBUG_RUNTIME ON)
    set(Boost_USE_STATIC_RUNTIME OFF)

    # Disable conflicting default libraries
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libcmtd.lib")
endif()
