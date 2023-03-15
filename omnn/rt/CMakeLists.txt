
set(DEPENDENCIES boost)
if(Boost_FOUND AND NOT MSVC)
    set(DEPENDENCIES ${DEPENDENCIES} Boost::filesystem)
endif()

if(OPENMIND_USE_OPENCL)
	set(DEPENDENCIES ${DEPENDENCIES} OpenCL)
endif()

lib(${DEPENDENCIES})

if(OPENMIND_USE_OPENCL)
    target_compile_definitions(${this_target} PUBLIC OPENMIND_MATH_USE_OPENCL)
endif()
