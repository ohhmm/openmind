include(find_local_package)


function(ext_configure_args name args)

	string(REPLACE "-" "_" name ${name})

	set(add_args
		-D${name}_TESTS=OFF
		-D${name}_BUILD_TESTS:BOOL=OFF
		-D${name}_BENCHMARK=OFF
		-D${name}_BENCHMARKS=OFF
		-D${name}_BENCHMARKING=OFF
		-D${name}_BUILD_BENCHMARK=OFF
		-D${name}_BUILD_BENCHMARKS=OFF
		-D${name}_BUILD_BENCHMARKING=OFF
		-D${name}_BUILD_GMOCK=OFF
		-D${name}_FRAMEWORK=${APPLE}
		-D${name}_STATIC=ON
		-D${name}_BUILD_APIDOC=OFF
		-D${name}_BUILD_DEMOS=OFF
		-D${name}_BUILD_EXAMPLES=OFF

		-DBENCHMARK_ENABLE_GTEST_TESTS:BOOL=OFF
		-DCMAKE_CXX_FLAGS=-D_CRT_SECURE_NO_WARNINGS
		)

	if(${name}_C_STANDARD)
		set(add_args ${add_args}
			-DCMAKE_C_STANDARD=${${name}_C_STANDARD}
			-DCMAKE_C_STANDARD_REQUIRED=ON
			)
	endif()

	if(_IS_DEBUG)
		set(add_args ${add_args}
			-D${name}_SHARED=ON
			)
	endif()

	if(${name}_CXX_STANDARD)
		set(add_args ${add_args}
			-DCMAKE_CXX_STANDARD=${${name}_CXX_STANDARD}
			-DCMAKE_CXX_STANDARD_REQUIRED=ON
			)
	endif()

	set(${args} ${${args}} ${add_args} PARENT_SCOPE)

endfunction()


macro(ext name location)
	message("External ${name} ${location}")
	if(NOT TARGET ${name})
		if(NOT OPENMIND_UPDATE_EXTERNAL_DEPS)
			set(OPENMIND_UPDATE_EXTERNAL_DEPS_CMD_PARAM UPDATE_COMMAND "")
		endif()
		if(MSVC)
			set(build_type -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}) # overcoming runtime mismatch linking issue
		else()
			set(build_type -DCMAKE_BUILD_TYPE=Release)
		endif()

		set(ARGS
			${build_type}
			-DBUILD_TESTS=OFF
			-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
			-DGIT_EXECUTABLE=${GIT_EXECUTABLE}
			-DHAVE_STD_REGEX=ON
			-DRUN_HAVE_STD_REGEX=1
			)

		ext_configure_args(${name} ARGS)
		string(TOUPPER ${name} _name_upcase)
		if(NOT name STREQUAL _name_upcase)
			ext_configure_args(${_name_upcase} ARGS)
		endif()

		STRING(REGEX REPLACE "^[Ll][Ii][Bb]" "" libless ${_name_upcase})
		if(NOT libless STREQUAL _name_upcase)
			ext_configure_args(${libless} ARGS)
		endif()

		if(APPLE)
			set(ARGS ${ARGS} -DBUILD_ZLIB=OFF)
		endif()
		if(CMAKE_SYSTEM_NAME)
			set(ARGS ${ARGS} -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME})
		endif()
		if(CMAKE_SYSTEM_PROCESSOR)
			set(ARGS ${ARGS} -DCMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR})
		endif()
		if(CMAKE_COMPILER_PREFIX)
			set(ARGS ${ARGS} -DCMAKE_COMPILER_PREFIX=${CMAKE_COMPILER_PREFIX})
		endif()
		if(CMAKE_FIND_ROOT_PATH)
			set(ARGS ${ARGS} -DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH})
		endif()
		if(OPENMIND_CMAKE_GENERATOR_FOR_${name})
			set(ARGS ${ARGS} -G=${OPENMIND_CMAKE_GENERATOR_FOR_${name}})
		elseif(OPENMIND_CMAKE_GENERATOR_FOR_${_name_upcase})
			set(ARGS ${ARGS} -G=${OPENMIND_CMAKE_GENERATOR_FOR_${_name_upcase}})
		elseif(OPENMIND_CMAKE_GENERATOR_FOR_DEPS)
			set(ARGS ${ARGS} -G=${OPENMIND_CMAKE_GENERATOR_FOR_DEPS})
		else()
			set(ARGS ${ARGS} -G=${CMAKE_GENERATOR}
				-DCMAKE_AR=${CMAKE_AR}
				-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
				-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
				-DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
				-DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}
				-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
				)
		endif()

		execute_process(
			COMMAND ${GIT_EXECUTABLE} ls-remote ${location} tags/*
			RESULT_VARIABLE RESULT
			OUTPUT_VARIABLE OUTPUT)

		set (DEFAULT_TAG HEAD)
		set (${name}_TAG ${DEFAULT_TAG} CACHE STRING "Select a repo tag")

		string (REGEX MATCHALL "([^/]+)[\^{}]\n" TAGS "${OUTPUT}")
		string (REGEX REPLACE "\n" ";"  TAGS "${TAGS}")
		set_property (CACHE ${name}_TAG PROPERTY STRINGS "${TAGS}")
		message ("ExternalProject_Add(${name}
		GIT_REPOSITORY ${location}
		GIT_SHALLOW ON
		GIT_TAG ${${name}_TAG}
		INSTALL_DIR ${CMAKE_BINARY_DIR}
		${OPENMIND_UPDATE_EXTERNAL_DEPS_CMD_PARAM}
		CMAKE_ARGS ${ARGS}
		)")
		ExternalProject_Add(${name}
			GIT_REPOSITORY ${location}
			GIT_SHALLOW ON
			GIT_TAG ${${name}_TAG}
			INSTALL_DIR ${CMAKE_BINARY_DIR}
			${OPENMIND_UPDATE_EXTERNAL_DEPS_CMD_PARAM}
			CMAKE_ARGS ${ARGS}
			)
		if(NOT TARGET ${name})
			add_custom_target(${name})
		endif()
		if(TARGET ${name})
			message("${name} is new target")
		    set_target_properties(${name} PROPERTIES FOLDER "util")
			add_dependencies(prerequisites ${name})
		endif()

		set(${name}_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include CACHE PATH "Path")
		if(MSVC)
			set(${name}_LIBRARIES ${CMAKE_BINARY_DIR}/lib/${name}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE PATH "Path")
		else()
			set(${name}_LIBRARIES ${CMAKE_BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${name}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE PATH "Path")
			#set(${name}_LIBRARIES ${CMAKE_BINARY_DIR}/lib64/${CMAKE_STATIC_LIBRARY_PREFIX}${name}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE PATH "Path")
		endif()

		if(TARGET ${this_target})
			if(${name}_INCLUDE_DIR)
				target_include_directories(${this_target} PUBLIC ${${name}_INCLUDE_DIR})
			endif()
			if(${name}_INCLUDE_DIRS)
				target_include_directories(${this_target} PUBLIC ${${name}_INCLUDE_DIRS})
			endif()
			target_link_directories(${this_target} PUBLIC
				${CMAKE_BINARY_DIR}/lib
				${CMAKE_BINARY_DIR}/lib64
				)
		else()
			if(${name}_INCLUDE_DIR)
				include_directories(${${name}_INCLUDE_DIR})
			endif()
			if(${name}_INCLUDE_DIRS)
				include_directories(${${name}_INCLUDE_DIRS})
			endif()
			link_directories(
				${CMAKE_BINARY_DIR}/lib
				${CMAKE_BINARY_DIR}/lib64
				)
		endif()
	else()
		message("${name} target exists")
	endif()
	if(OPENMIND_DEPEND_ON_PREREQUISITES AND OPENMIND_BUILD_BOOST)
		add_dependencies(${dep} boost)
	endif()
endmacro()


macro(dep)
	message("dep ARGN: ${ARGN}")
	if(${ARGN} MATCHES " ")
		string(REPLACE " " ";" ARGN ${ARGN})
	endif()
	foreach(dep ${ARGN})

		message("dep: ${dep}")
		if(NOT TARGET ${dep} AND IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${dep})
			# TODO: waiting for CMake team to resolve this
			#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../${dep} ${CMAKE_CURRENT_BINARY_DIR}/../${dep})
		endif()

		if(TARGET ${dep})
			get_target_property(type ${dep} TYPE)
			message("'${dep}' target dependency of '${type}' type for '${this_target}' is ready")
			if(${type} MATCHES "STATIC_LIBRARY|MODULE_LIBRARY|SHARED_LIBRARY")
				message("'${dep}'  MATCHES STATIC_LIBRARY|MODULE_LIBRARY|SHARED_LIBRARY")
				target_link_libraries(${this_target} PUBLIC ${dep})
			else()
				message("'${dep}' target type is '${type}'")
				if(OPENMIND_DEPEND_ON_PREREQUISITES AND NOT type STREQUAL UTILITY)
					add_dependencies(${this_target} ${dep})
				endif()
			endif()
			if(CHECK_IF_NEXT_ITEM_IS_MATCHING_TARGET)
				if(dep MATCHES ${CHECK_IF_NEXT_ITEM_IS_MATCHING_TARGET})
					set(${CHECK_IF_NEXT_ITEM_IS_MATCHING_TARGET}_LIBRARIES "${dep}" CACHE STRING "${CHECK_IF_NEXT_ITEM_IS_MATCHING_TARGET} dependency target" FORCE)
					set(libs ${${this_target}_LIBRARIES})
					list(REMOVE_ITEM libs ${CMAKE_BINARY_DIR}/lib/${CHECK_IF_NEXT_ITEM_IS_MATCHING_TARGET}${CMAKE_STATIC_LIBRARY_SUFFIX})
					list(REMOVE_ITEM libs ${CMAKE_BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${CHECK_IF_NEXT_ITEM_IS_MATCHING_TARGET}${CMAKE_STATIC_LIBRARY_SUFFIX})
					set(${this_target}_LIBRARIES "${libs}" CACHE PATH "${this_target} libs & dep targets to carry" FORCE)
				endif()
			endif()
		elseif(dep MATCHES ::)
			message("dep ${dep} target isn't ready")
			STRING(REGEX REPLACE "::\.+" "" libname ${dep})
			if(NOT ${libname}_FOUND)
				message("trying ${libname} library")
				dep(${libname})
				if(${libname}_FOUND)
					message("found ${libname} library")
				endif()
			endif()
			if(NOT TARGET ${dep})
				STRING(REGEX REPLACE "\.+::" "" componentname ${dep})
				message("searching for ${componentname} component in ${libname} library")
				find_pkg(${libname})
				if(${libname}_FOUND)
					find_package(${libname} COMPONENTS ${componentname})
				endif()
			endif()
			if(TARGET ${dep})
				dep(${dep})
			endif()
		elseif(${dep} STREQUAL pthread)
			message("dep ${dep} is -pthread option")
		elseif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${dep})
			message(ERROR "dep ${dep} target is yet to be processed sibling directory library (add param fold(${dep}))")
		elseif(dep MATCHES "((git|ssh|http(s)?)|(git@[-(A-Z)|(a-z)|(0-9)\.]+))(:(//)?)([-(A-Z)|(a-z)|(0-9)\.@\:/\-~]+)(\.git)?(/)?")
			set(repo ${dep})
			if(dep MATCHES /$)
				string(REGEX REPLACE "/$" "" dep "${dep}")
			endif()
			get_filename_component(dep "${dep}" NAME_WE)
			message("${repo} is link to repository for ${dep}")
			dep_find_package()
			if(NOT ${dep}_FOUND)
				ext(${dep} ${repo})
				message("Dependency ${dep} took from ${repo}")
				if(dep MATCHES [qQ]t)
					set(OPENMIND_USE_QT ON CACHE BOOL "Qt used is enabled for dependency ${dep}" FORCE)
				endif()
			endif()
			set(CHECK_IF_NEXT_ITEM_IS_MATCHING_TARGET ${dep})
			continue()
		elseif(dep MATCHES /)
			if(dep MATCHES .git$)
				string(REGEX REPLACE ".git$" "" dep "${dep}")
			endif()
			dep("https://github.com/${dep}.git")
			get_filename_component(dep "${dep}" NAME_WE)
			set(CHECK_IF_NEXT_ITEM_IS_MATCHING_TARGET ${dep})
			continue()
		else()
			message("Assuming the ${dep} is preinstalled")
			dep_find_package()
		endif()
		unset(CHECK_IF_NEXT_ITEM_IS_MATCHING_TARGET)
	endforeach()
endmacro()

macro(deps)
    string(STRIP "${ARGN}" dependencies_stripped)
	string(REPLACE " " ";" dependencies_listified "${dependencies_stripped}")
	message("foreaching ${deps}")
	set(this_target_libs "")
    foreach(tg ${dependencies_listified})
	    dep(${tg})
		if(TARGET ${tg})
			get_target_property(this_target_type ${tg} TYPE)
			if(NOT this_target_type STREQUAL UTILITY)
				message("dep on target ${tg}")
				if(OPENMIND_DEPEND_ON_PREREQUISITES)
					add_dependencies(${this_target} ${tg})
				endif()
				list(APPEND this_target_libs ${tg})
			endif()
		elseif(tg MATCHES ::)
			message("Skipping ${tg} which is to be installed with prerequisites yet")
		elseif(EXISTS ${tg})
			list(APPEND this_target_libs ${tg})
		elseif(${tg}_LIBRARY)
			list(APPEND this_target_libs ${${tg}_LIBRARY})
		else()
			get_filename_component(t ${tg} NAME_WLE)
			message("its ${t}")
			if(TARGET ${t})
				get_target_property(this_target_type ${t} TYPE)
				message("dep on target ${t}")
				if(NOT this_target_type STREQUAL UTILITY)
					if(OPENMIND_DEPEND_ON_PREREQUISITES)
						add_dependencies(${this_target} ${t})
					endif()
					list(APPEND this_target_libs ${t})
				elseif(${t}_LIBRARIES)
					list(APPEND this_target_libs ${${t}_LIBRARIES})
				endif()
			elseif(${t} MATCHES "^lib.+")
				STRING(REGEX REPLACE "^lib(.+)" "\\1"  tg ${t})
				if(TARGET ${tg})
					get_target_property(this_target_type ${tg} TYPE)
					if(NOT this_target_type STREQUAL UTILITY)
						message("${t} -> dep on target ${tg}")
						if(OPENMIND_DEPEND_ON_PREREQUISITES)
							add_dependencies(${this_target} ${tg})
						endif()
						list(APPEND this_target_libs ${tg})
					endif()
				endif()
			endif()
		endif()
	endforeach()

    set(${this_target}_LIBRARIES ${this_target_libs} CACHE FILEPATH "${this_target} libs to link"
		#FORCE
		)
    message("target_link_directories(${this_target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})")
    target_link_directories(${this_target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    if(this_target_libs AND NOT this_target_libs STREQUAL "")
        message("${this_target} libs: ${this_target_libs}")
		foreach(lib ${this_target_libs})
			if(TARGET ${lib})
				get_target_property(lib_target_type ${lib} TYPE)
				message("${lib} is ${lib_target_type} type target")
				if(lib_target_type STREQUAL UTILITY)
					if(OPENMIND_DEPEND_ON_PREREQUISITES)
						add_dependencies(${this_target} ${lib})
					endif()
					continue()
				endif()
			endif()
			if(NOT lib STREQUAL "optimized"
				AND NOT lib STREQUAL "debug"
				)
				message("lib: ${lib}")
				target_link_libraries(${this_target} PUBLIC ${lib})
			endif()
		endforeach()
    else()
        message("${this_target} has no libs")
    endif()
    if(NOT MSVC)
        target_link_libraries(${this_target} PUBLIC pthread)
		if(NOT APPLE)
	        target_link_libraries(${this_target} PUBLIC ${CMAKE_DL_LIBS})
		endif()
    endif()

endmacro()

