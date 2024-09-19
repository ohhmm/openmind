
if (OPENMIND_USE_VCPKG)
    set(DEPENDENCIES
            Boost::filesystem
            Boost::multiprecision
            Boost::system
    )
    if(OPENMIND_BUILD_TESTS)
        list(APPEND DEPENDENCIES Boost::unit_test_framework)
    endif()
else ()
    set(DEPENDENCIES boost)
    if(Boost_FOUND)
        if(OPENMIND_USE_VCPKG)
            set(DEPENDENCIES ${DEPENDENCIES}
                Boost::compute
                Boost::headers
                Boost::filesystem
                Boost::multiprecision
            )
        elseif(NOT MSVC OR OPENMIND_USE_CONAN)
            list(APPEND DEPENDENCIES Boost::filesystem)
            if(OPENMIND_BUILD_TESTS)
                list(APPEND DEPENDENCIES
                        Boost::test_exec_monitor
                        Boost::unit_test_framework
                )
            endif()
        endif()
    endif()
endif()

if(OPENMIND_USE_OPENCL)
    list(APPEND DEPENDENCIES OpenCL)
    if (OPENMIND_USE_VCPKG)
        list(APPEND DEPENDENCIES Boost::compute)
    endif()
endif()

lib(${DEPENDENCIES})
