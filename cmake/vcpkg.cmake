
macro(find_vcpkg)
    if(NOT EXISTS ${CMAKE_TOOLCHAIN_FILE})
        if(NOT EXISTS ${VCPKG_EXECUTABLE})
	        find_program(VCPKG_EXECUTABLE vcpkg PATHS
		        "$ENV{ProgramFiles}/Microsoft Visual Studio/*/*/VC/vcpkg/"
		        "$ENV{USERPROFILE}$ENV{HOME}/vcpkg/"
	        )
	        if (EXISTS ${VCPKG_EXECUTABLE})
		        get_filename_component(dir ${VCPKG_EXECUTABLE} DIRECTORY)
		        set(CMAKE_TOOLCHAIN_FILE "${dir}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
	        endif ()
        endif()
        if(EXISTS ${CMAKE_TOOLCHAIN_FILE})
            set(VCPKG_FOUND TRUE CACHE BOOL "VCPKG toolchain file found ${CMAKE_TOOLCHAIN_FILE}")
        endif()
    endif()
endmacro()

macro(fetch_vcpkg)
	include(gitect)
	if(GIT_EXECUTABLE)
		execute_process(
			COMMAND ${GIT_EXECUTABLE} clone https://github.com/microsoft/vcpkg
				WORKING_DIRECTORY $ENV{USERPROFILE}$ENV{HOME}
			)
		execute_process(
			COMMAND ${CMAKE_COMMAND} --build .
			WORKING_DIRECTORY ${vcpkg_SOURCE_DIR}
		)
	endif()
endmacro()

if(NOT ${VCPKG_FOUND})
	find_vcpkg()
endif()

option(OPENMIND_USE_VCPKG "Use vcpkg" ${VCPKG_FOUND})

if(OPENMIND_USE_VCPKG)
    if(NOT ${VCPKG_FOUND})
		find_vcpkg()
	endif()
    if(NOT ${VCPKG_FOUND})
		fetch_vcpkg()
		find_vcpkg()
	endif()
    if(NOT ${VCPKG_FOUND})
		message(FATAL_ERROR "VCPKG is required but not found")
	endif()
	if(VCPKG_EXECUTABLE)
		execute_process(
			COMMAND ${VCPKG_EXECUTABLE} install
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		)
	endif()
endif (OPENMIND_USE_VCPKG)
