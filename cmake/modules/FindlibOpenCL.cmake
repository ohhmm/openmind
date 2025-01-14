# Find OpenCL
include(FindPackageHandleStandardArgs)

# Skip OpenCL check in CI or if explicitly requested
if(DEFINED ENV{SKIP_OPENCL_CHECK} OR DEFINED ENV{CI})
    set(libOpenCL_FOUND TRUE)
    set(libOpenCL_INCLUDE_DIRS "")
    set(libOpenCL_LIBRARIES "")
    set(OPENMIND_USE_OPENCL OFF CACHE BOOL "Use OpenCL" FORCE)
else()
    if(NOT libOpenCL_FOUND)
        find_package(OpenCL QUIET)
        if(OpenCL_FOUND)
            set(libOpenCL_FOUND TRUE)
            set(libOpenCL_INCLUDE_DIRS ${OpenCL_INCLUDE_DIRS})
            set(libOpenCL_LIBRARIES ${OpenCL_LIBRARIES})
            set(OPENMIND_USE_OPENCL ON CACHE BOOL "Use OpenCL" FORCE)
        else()
            set(OPENMIND_USE_OPENCL OFF CACHE BOOL "Use OpenCL" FORCE)
        endif()
    endif()
endif()

find_package_handle_standard_args(libOpenCL
    REQUIRED_VARS libOpenCL_FOUND
)

if(libOpenCL_FOUND AND NOT TARGET OpenCL::OpenCL)
    add_library(OpenCL::OpenCL INTERFACE IMPORTED)
    if(libOpenCL_INCLUDE_DIRS)
        set_target_properties(OpenCL::OpenCL PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${libOpenCL_INCLUDE_DIRS}"
        )
    endif()
    if(libOpenCL_LIBRARIES)
        set_target_properties(OpenCL::OpenCL PROPERTIES
            INTERFACE_LINK_LIBRARIES "${libOpenCL_LIBRARIES}"
        )
    endif()
endif()
