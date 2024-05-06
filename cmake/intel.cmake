
if(NOT EXISTS INTEL_COMPILER_PATH)
	find_program(INTEL_COMPILER_PATH icpx)
	if(INTEL_COMPILER_PATH)
		message("INTEL_COMPILER_PATH: ${INTEL_COMPILER_PATH}")
		set(CMAKE_CXX_COMPILER ${INTEL_COMPILER_PATH} CACHE STRING "Intel compiler path" FORCE)
	endif()
endif()
if(NOT IntelSYCL_FOUND)
	if(NOT DEFINED OPENMIND_USE_OPENCL_INTEL_SYCL)
		option(OPENMIND_USE_OPENCL_INTEL_SYCL "Use Intel SYCL" ${IntelSYCL_FOUND})
		find_package(IntelSYCL)
		if(IntelSYCL_FOUND)
			set(OPENMIND_USE_OPENCL_INTEL_SYCL ON CACHE BOOL "OpenCL GPU calculations" FORCE)
		endif()
	endif()
	if(IntelSYCL_FOUND)
		message("IntelSYCL_FOUND: ${IntelSYCL_FOUND}")
		message("SYCL_FLAGS: ${SYCL_FLAGS}")
		message("IntelSYCL_DIR: ${IntelSYCL_DIR}")
	endif()
endif()
find_optional_pkg(
	IntelDPCPP
	CUDAToolkit
	onnxruntime
	OpenCL
)

if(IntelSYCL_FOUND AND OPENMIND_USE_OPENCL_INTEL_SYCL
	AND NOT EXISTS OpenCL_INCLUDE_DIR
	AND EXISTS IntelSYCL_DIR
)
	set(OpenCL_INCLUDE_DIR "${IntelSYCL_DIR}/../../../include/sycl" CACHE PATH "IntelSYCL is used for OpenCL" FORCE)
endif()
