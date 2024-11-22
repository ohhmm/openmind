include(pytect)

macro(check_dep_file)
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
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt)
		message("Conan deps: ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt")
		option(OPENMIND_USE_CONAN "Use conan dependencies" ON)
		message("Use conan dependencies")
		if(NOT EXISTS CONAN_EXECUTABLE)
			unset(CONAN_EXECUTABLE CACHE)
		endif()
		if(NOT CONAN_EXECUTABLE)
			set(PYTHON_SEARCH_PATHS $ENV{USERPROFILE}$ENV{HOME}/.local/bin)
			if(EXISTS "$ENV{APPDATA}/Python/")
				list(PYTHON_SEARCH_PATHS APPEND "$ENV{APPDATA}/Python/*/Scripts")
			endif()
			find_program(CONAN_EXECUTABLE NAMES conan PATHS ${PYTHON_SEARCH_PATHS})
		endif()
		if(NOT CONAN_EXECUTABLE)
			if(EXISTS ${Python_EXECUTABLE})
				EXECUTE_PROCESS(
					COMMAND ${Python_EXECUTABLE} -m pip install conan
					OUTPUT_VARIABLE pipConanOutput
					ERROR_VARIABLE pipConanError
					OUTPUT_STRIP_TRAILING_WHITESPACE
					COMMAND_ECHO STDOUT
					)
						
				string(REGEX MATCH "WARNING\: The script.+ installed in '(.+)' which is not on PATH\." warnBinPath "${pipConanError}")
				if(warnBinPath)
					message("${warnBinPath}")
					string(REPLACE "'" ";" tokenized_string ${warnBinPath})
					list(GET tokenized_string 1 warnBinPath)
				else()
					message("pip output: ${pipConanOutput}")
					unset(warnBinPath)
				endif()
				list(APPEND PYTHON_SEARCH_PATHS "${warnBinPath}")
				find_program(CONAN_EXECUTABLE NAMES conan PATHS ${PYTHON_SEARCH_PATHS})
				if(CONAN_EXECUTABLE AND warnBinPath)
					EXECUTE_PROCESS(
						COMMAND ${CONAN_EXECUTABLE} profile detect
						COMMAND_ECHO STDOUT
						)
				endif()
			endif()
		endif()
		if(CONAN_EXECUTABLE)
			message("Using ${CONAN_EXECUTABLE} to create util/Install_${this_target}_Conan dependency target for prerequisites target")
			add_custom_target(Install_${this_target}_Conan
				WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
				COMMAND ${CONAN_EXECUTABLE} install ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt -b=${CMAKE_CURRENT_BINARY_DIR} --build=missing -g CMakeDeps
				COMMENT "Installing Conan dependencies from ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt"
				SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt
				DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt
			)
			set_target_properties(Install_${this_target}_Conan PROPERTIES
				FOLDER "util"
			)
			if(TARGET Install_${this_target}_Dependencies)
				add_dependencies(Install_${this_target}_Dependencies Install_${this_target}_Conan)
			else()
				add_dependencies(prerequisites Install_${this_target}_Conan)
			endif()
			set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
				"${CMAKE_CURRENT_SOURCE_DIR}"
				"${CMAKE_CURRENT_BINARY_DIR}"
				)
			set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_MODULE_PATH})
		else(CONAN_EXECUTABLE)
			message("CONAN_EXECUTABLE not found")
		endif(CONAN_EXECUTABLE)
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
