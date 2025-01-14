# Find Intel SYCL
include(FindPackageHandleStandardArgs)

if(NOT IntelSYCL_FOUND)
    find_package(IntelDPCPP QUIET)
    if(IntelDPCPP_FOUND)
        set(IntelSYCL_FOUND TRUE)
    endif()
endif()

find_package_handle_standard_args(IntelSYCL
    REQUIRED_VARS IntelSYCL_FOUND
)
