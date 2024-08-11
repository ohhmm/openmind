option(OPENMIND_USE_VCPKG "Use vcpkg" OFF)
if(OPENMIND_USE_VCPKG)
    if(NOT EXISTS ${VCPKG_EXECUTABLE})
        find_program(VCPKG_EXECUTABLE vcpkg PATHS
            "$ENV{ProgramFiles}/Microsoft Visual Studio/*/*/VC/vcpkg/"
            "$ENV{USERPROFILE}$ENV{HOME}/vcpkg/"
        )
    endif()

    if (EXISTS ${VCPKG_EXECUTABLE})
        get_filename_component(dir ${VCPKG_EXECUTABLE} DIRECTORY)
        set(CMAKE_TOOLCHAIN_FILE "${dir}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    endif ()
endif (OPENMIND_USE_VCPKG)
