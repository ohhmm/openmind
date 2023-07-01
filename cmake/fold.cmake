
macro(check_dep_file)
	if(NOT Python_FOUND)
		find_package(Python)
	endif()
	if(Python_EXECUTABLE AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt)
		add_custom_target(Install_${this_target}_Dependencies
			COMMAND ${Python_EXECUTABLE} -m pip install --user -r requirements.txt
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			COMMENT "Installing Python dependencies from ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
			SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
		)
		set_target_properties(Install_${this_target}_Dependencies PROPERTIES
			FOLDER "util"
		)
		add_dependencies(prerequisites Install_${this_target}_Dependencies)
	endif()
endmacro(check_dep_file)

macro(platform_specific_iteration) # FIXME: CMake hasn't support for the continue() out of macro
	message("IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}")
    IF(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}
		AND ((${ARGN} STREQUAL "windows" AND NOT WIN32)
			OR (${ARGN} STREQUAL "apple" AND NOT APPLE)
			OR (${ARGN} STREQUAL "linux" AND (WIN32 OR APPLE)))
	)
		CONTINUE() # FIXME: CMake hasn't support for the continue() out of macro
	ENDIF()
endmacro(platform_specific_iteration)

macro(fold)
    get_filename_component(this_project ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    project(${this_project})
    message("\nCreating Project: ${this_project}")
    string(STRIP "${ARGN}" first)
    file(GLOB items *)
    if(first)
        message("Folding ${CMAKE_CURRENT_SOURCE_DIR} (${first} first)")
    else(first)
        message("Folding ${CMAKE_CURRENT_SOURCE_DIR}")
    endif(first)
    foreach(name ${first})
		#platform_specific_iteration(${name})
		IF((name STREQUAL "windows" AND NOT WIN32)
			OR (name STREQUAL "apple" AND NOT APPLE)
			OR (name STREQUAL "linux" AND (WIN32 OR APPLE))
		)
			CONTINUE()
		ENDIF()
        set(item ${CMAKE_CURRENT_SOURCE_DIR}/${name})
        IF(IS_DIRECTORY ${item} AND EXISTS ${item}/CMakeLists.txt)
			message("${item} is dir with CMakeLists.txt")
			add_subdirectory(${item})
			list(REMOVE_ITEM items ${item})
        endif()
    endforeach()

    foreach(item ${items})
        IF(IS_DIRECTORY ${item} AND EXISTS ${item}/CMakeLists.txt)
			get_filename_component(name ${item} NAME)
			IF(NOT ${name} STREQUAL tests)
	            message("${item} is dir with CMakeLists.txt")
    	        get_filename_component(name ${item} NAME)
				#platform_specific_iteration(${name})
				IF((name STREQUAL "windows" AND NOT WIN32)
					OR (name STREQUAL "apple" AND NOT APPLE)
					OR (name STREQUAL "linux" AND (WIN32 OR APPLE))
				)
					CONTINUE()
				ENDIF()
				add_subdirectory(${item})
			endif()
		endif()
    endforeach()

	check_dep_file()
endmacro()
