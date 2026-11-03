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

if(NOT OPENMIND_BUILD_GC)
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/GC.cpp" PROPERTIES HEADER_FILE_ONLY ON)
endif()

set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/tasq.cpp" PROPERTIES HEADER_FILE_ONLY ON)

lib(${DEPENDENCIES})
