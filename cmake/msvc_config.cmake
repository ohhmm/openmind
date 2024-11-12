cmake
if(MSVC)
    # Force consistent runtime library settings first
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /D_ITERATOR_DEBUG_LEVEL=0")
        add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /D_ITERATOR_DEBUG_LEVEL=2")
        add_definitions(-D_ITERATOR_DEBUG_LEVEL=2)
    endif()

    # Boost configuration
    set(Boost_USE_STATIC_RUNTIME OFF)
    set(Boost_USE_DEBUG_RUNTIME ON)

    # Disable conflicting default libraries
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libcmtd.lib")

    # Force consistent runtime settings across all targets
    foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MT")
            string(REGEX REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()
