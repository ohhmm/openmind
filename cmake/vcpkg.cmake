
function(prepend_toolchain_file)
	if(EXISTS ${CMAKE_TOOLCHAIN_FILE})
		list(PREPEND CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_TOOLCHAIN_FILE}")
		set(${CMAKE_PROJECT_TOP_LEVEL_INCLUDES} "${${CMAKE_PROJECT_TOP_LEVEL_INCLUDES}}" PARENT_SCOPE)
		list(PREPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES CMAKE_PROJECT_TOP_LEVEL_INCLUDES)
		set(${CMAKE_TRY_COMPILE_PLATFORM_VARIABLES} "${${CMAKE_TRY_COMPILE_PLATFORM_VARIABLES}}" PARENT_SCOPE)
	endif()
endfunction()

macro(find_vcpkg)
    if(NOT EXISTS ${CMAKE_TOOLCHAIN_FILE})
        if(NOT EXISTS ${VCPKG_EXECUTABLE})
	        find_program(VCPKG_EXECUTABLE vcpkg PATHS
		        "$ENV{ProgramFiles}/Microsoft Visual Studio/*/*/VC/vcpkg/"
		        "$ENV{USERPROFILE}$ENV{HOME}/vcpkg/"
	        )
        endif()
        if (EXISTS ${VCPKG_EXECUTABLE})
            message("VCPKG_EXECUTABLE=${VCPKG_EXECUTABLE}")
            get_filename_component(VCPKG_ROOT ${VCPKG_EXECUTABLE} DIRECTORY)
            set(VCPKG_ROOT "${VCPKG_ROOT}" CACHE PATH "Path to vcpkg installation" FORCE)
            set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file" FORCE)
        endif()
    endif()

    if(EXISTS ${CMAKE_TOOLCHAIN_FILE})
        set(VCPKG_FOUND TRUE CACHE BOOL "VCPKG toolchain file found ${CMAKE_TOOLCHAIN_FILE}" FORCE)
        #prepend_toolchain_file()
    else()
        set(VCPKG_FOUND FALSE CACHE BOOL "VCPKG not found" FORCE)
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
else()
	#prepend_toolchain_file()
endif()

option(OPENMIND_USE_VCPKG "Use vcpkg" ${VCPKG_FOUND})

if(OPENMIND_USE_VCPKG)
    if(NOT VCPKG_FOUND)
        find_vcpkg()
        if(NOT VCPKG_FOUND)
            fetch_vcpkg()
            if(NOT VCPKG_FOUND)
                find_vcpkg()
            endif()
        endif()
    endif()
    if(NOT VCPKG_FOUND)
        message(WARNING "Vcpkg not found")
    endif()
endif()
