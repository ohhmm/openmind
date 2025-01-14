# Find CUDA Toolkit
include(FindPackageHandleStandardArgs)

if(NOT libCUDAToolkit_FOUND)
    find_package(CUDAToolkit QUIET)
    if(CUDAToolkit_FOUND)
        set(libCUDAToolkit_FOUND TRUE)
        set(libCUDAToolkit_INCLUDE_DIRS ${CUDAToolkit_INCLUDE_DIRS})
        set(libCUDAToolkit_LIBRARIES ${CUDAToolkit_LIBRARIES})
    endif()
endif()

find_package_handle_standard_args(libCUDAToolkit
    REQUIRED_VARS libCUDAToolkit_FOUND
)
