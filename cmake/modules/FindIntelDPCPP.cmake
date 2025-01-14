# Find Intel DPC++
include(FindPackageHandleStandardArgs)

if(DEFINED ENV{DPCPPROOT})
    set(IntelDPCPP_ROOT $ENV{DPCPPROOT})
endif()

find_path(IntelDPCPP_INCLUDE_DIR
    NAMES CL/sycl.hpp
    PATHS
        ${IntelDPCPP_ROOT}/include
        /usr/include
        /usr/local/include
)

find_package_handle_standard_args(IntelDPCPP
    REQUIRED_VARS IntelDPCPP_INCLUDE_DIR
)

if(IntelDPCPP_FOUND)
    set(IntelDPCPP_INCLUDE_DIRS ${IntelDPCPP_INCLUDE_DIR})
endif()
