
macro(find_vcpkg)
    if(NOT EXISTS ${CMAKE_TOOLCHAIN_FILE})
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
        if(EXISTS ${CMAKE_TOOLCHAIN_FILE})
            set(VCPKG_FOUND TRUE CACHE BOOL "VCPKG toolchain file found ${CMAKE_TOOLCHAIN_FILE}")
			list(APPEND CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_TOOLCHAIN_FILE}")
			list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES CMAKE_PROJECT_TOP_LEVEL_INCLUDES)
        endif()
    endif()
endmacro()

macro(fetch_vcpkg)
	include(gitect)
	include(FetchContent)
	FetchContent_Populate(	VCPKG
		GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
		GIT_TAG        703a8113f42e794534df5dfc5cece5dabcb949d0
		SOURCE_DIR     "$ENV{USERPROFILE}$ENV{HOME}/vcpkg"
	)

	# if(GIT_EXECUTABLE)
	# 	execute_process(
	# 		COMMAND ${GIT_EXECUTABLE} clone https://github.com/microsoft/vcpkg
	# 			WORKING_DIRECTORY $ENV{USERPROFILE}$ENV{HOME}
	# 		)
	# 	execute_process(
	# 		COMMAND ${CMAKE_COMMAND} --build .
	# 		WORKING_DIRECTORY ${vcpkg_SOURCE_DIR}
	# 	)
	# endif()
endmacro()

if(NOT VCPKG_FOUND)
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
