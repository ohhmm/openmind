set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (OPENMIND_USE_VCPKG)
    set(DEPENDENCIES
            Boost::filesystem
            Boost::multiprecision
            Boost::system
    )
else ()
    set(DEPENDENCIES boost)
    if(Boost_FOUND)
        if(NOT MSVC OR OPENMIND_USE_CONAN)
            list(APPEND DEPENDENCIES Boost::filesystem)
        endif()
    endif()
endif()

if(OPENMIND_USE_OPENCL)
    list(APPEND DEPENDENCIES OpenCL)
    if (OPENMIND_USE_VCPKG)
        list(APPEND DEPENDENCIES Boost::compute)
    endif()
endif()

# Add stdc++ explicitly to ensure proper linkage
list(APPEND DEPENDENCIES stdc++)

lib(${DEPENDENCIES})
