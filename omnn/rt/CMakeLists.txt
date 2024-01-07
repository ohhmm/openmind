
set(DEPENDENCIES boost)
if(Boost_FOUND AND NOT MSVC)
    set(DEPENDENCIES ${DEPENDENCIES} Boost::filesystem)
    if(OPENMIND_BUILD_TESTS)
        set(DEPENDENCIES ${DEPENDENCIES}
            Boost::test_exec_monitor
            Boost::unit_test_framework
            )
    endif()
endif()

if(OPENMIND_USE_OPENCL)
	set(DEPENDENCIES ${DEPENDENCIES} OpenCL)
endif()

lib(${DEPENDENCIES})
