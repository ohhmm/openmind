cmake_minimum_required(VERSION 3.14)

set(PRODUCT_NAME OpenMind)
project(${PRODUCT_NAME} CXX)
enable_language(CXX)
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message("SRC_DIR=${CMAKE_SOURCE_DIR}/")

set(OPENMIND_PRIME_TABLE_OPTIONS Boost Bundled)
if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
    set(OPENMIND_PRIME_TABLE Boost CACHE STRING "Table of predefined prime numbers")
	set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} CACHE PATH "hint for find_package")
    set(_IS_DEBUG TRUE)
    #add_definitions(-DBOOST_ALL_DYN_LINK)
    if(MSVC)
        add_compile_options($<$<CONFIG:Debug>:/constexpr:steps1000000000>)
        add_link_options(/stack:1000000000)
    endif()
else()
    set(_IS_DEBUG FALSE)
    if(CMAKE_BUILD_TYPE MATCHES [Rr][Ee][Ll][Ee][Aa][Ss][Ee])
        set(OPENMIND_PRIME_TABLE Bundled CACHE STRING "Table of predefined prime numbers")
        add_definitions(-DBOOST_ALL_STATIC_LINK)
        if(MSVC)
            add_compile_options($<$<CONFIG:Release>:/MT>)
        endif()
    else()
        set(OPENMIND_PRIME_TABLE Boost CACHE STRING "Table of predefined prime numbers")
        message(WARNING "Consider setting CMAKE_BUILD_TYPE")
    endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/"
	"${CMAKE_BIN_DIR}/lib/cmake"
	"${CMAKE_SOURCE_DIR}/cmake"
	)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/qtect.cmake)
option(OPENMIND_BUILD_TESTS "Build various TESTS" ${_IS_DEBUG})
option(OPENMIND_DEBUG_CHECKS "Additional debug checks" ${_IS_DEBUG})
option(OPENMIND_BUILD_SAMPLES "Build various samples" FALSE)
option(OPENMIND_BUILD_GC "Use garbage collection. Faster but requires abundant memory." FALSE) 
option(OPENMIND_BUILD_3RD_PARTY_TESTS "Build various 3rd party TESTS to compare to openmind by performance" FALSE)
option(OPENMIND_PRIME_MINING "Extend primes table by appending to Primes.inc source code file during CPU cores idle" ${_IS_DEBUG})
option(OPENMIND_UPDATE_EXTERNAL_DEPS "Update ExternalProject dependencies" FALSE)
option(OPENMIND_USE_QT "Link Qt libs" OFF)
if(OPENMIND_USE_QT)
	qtect()
endif()
find_package(PkgConfig)
option(OPENMIND_USE_PKG_CONFIG_DEPS "Use pkg-config dependencies" ${PKG_CONFIG_FOUND})
if(PKG_CONFIG_FOUND)
	message("pkg-config may be used to detect some of dependencies")
endif()
find_package(OpenCL)
option(OPENMIND_USE_OPENCL "Use OpenCL calculations" ${OpenCL_FOUND})
if(OPENMIND_USE_OPENCL)
    add_definitions(-DOPENMIND_USE_OPENCL)
endif(OPENMIND_USE_OPENCL)
set(OPENMIND_PREFERRED_BOOST_VERSION 1.80.0 CACHE STRING "Boost library version to use")
set(OPENMIND_REQUIRED_BOOST_VERSION 1.80.0 CACHE STRING "Boost library minimum compatible version to use")
set_property(CACHE OPENMIND_PRIME_TABLE PROPERTY STRINGS ${OPENMIND_PRIME_TABLE_OPTIONS})

if(MSVC AND ${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24.0")
    message("Using Edit&Continue policy")
    cmake_policy(SET CMP0138 NEW)
endif()
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.19.0")
    cmake_policy(SET CMP0114 NEW)
endif()

if(OPENMIND_PRIME_TABLE STREQUAL Boost)
	add_definitions(-DOPENMIND_PRIME_TABLE_BOOST)
elseif(OPENMIND_PRIME_TABLE STREQUAL Bundled)
	add_definitions(-DOPENMIND_PRIME_TABLE_OM)
endif()
set(BOOST_USED_COMPONENTS
	${BOOST_USED_COMPONENTS}
	${BOOST_ADDITIONAL_COMPONENTS}
	date_time
	filesystem
	iostreams
	serialization
	system
	thread)
if(OPENMIND_BUILD_TESTS OR _IS_DEBUG OR NOT CMAKE_BUILD_TYPE)
    set(BOOST_USED_COMPONENTS ${BOOST_USED_COMPONENTS}
        unit_test_framework
        test_exec_monitor
        )
endif()
list(REMOVE_DUPLICATES BOOST_USED_COMPONENTS)
set(BOOST_USED_COMPONENTS ${BOOST_USED_COMPONENTS} CACHE STRING "Components")

if(OPENMIND_BUILD_GC)
	add_definitions(-DOPENMIND_BUILD_GC)
endif()
if(OPENMIND_PRIME_MINING)
	add_definitions(-DOPENMIND_PRIME_MINING)
endif()

set(OPENMIND_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message("OPENMIND_INCLUDE_DIR ${OPENMIND_INCLUDE_DIR}")

if(WIN32)
    find_package(Boost ${OPENMIND_REQUIRED_BOOST_VERSION}
		CONFIG
		PATHS C:/Boost
		COMPONENTS ${BOOST_USED_COMPONENTS}
		)
else()
    find_package(Boost ${OPENMIND_REQUIRED_BOOST_VERSION}
		CONFIG
		COMPONENTS ${BOOST_USED_COMPONENTS}
		)
endif()

add_custom_target(prerequisites)
set_target_properties(prerequisites PROPERTIES FOLDER "util")


if(NOT Boost_FOUND)
    message("Building Boost ${OPENMIND_PREFERRED_BOOST_VERSION}")
    set(OM_BUILD_BOOST True)
else()
	message("Boost_VERSION: ${Boost_VERSION}")
	message("Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
	message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
	message("BOOST_USED_COMPONENTS: ${BOOST_USED_COMPONENTS}")
	message("Boost_LIBRARIES: ${Boost_LIBRARIES}")

    foreach(_module ${BOOST_USED_COMPONENTS})
		string(TOUPPER ${_module} _module_upcase)
		message("Boost_${_module_upcase}_FOUND: ${Boost_${_module_upcase}_FOUND}")
		message("Boost_${_module_upcase}_LIBRARY: ${Boost_${_module_upcase}_LIBRARY}")
	endforeach()
endif()

option(OPENMIND_BUILD_BOOST "Fetch&build boost" ${OM_BUILD_BOOST})

include(CPack)
include(ExternalData)
include(ExternalProject)

find_package(Git QUIET)
if(NOT GIT_EXECUTABLE AND WIN32)
	find_program(GIT_EXECUTABLE git PATHS 
		"$ENV{ProgramFiles}/Microsoft Visual Studio/*/*/Common7/IDE/CommonExtensions/Microsoft/TeamFoundation/Team Explorer/Git/cmd/"
		)
	find_package(Git)
endif()
if(GIT_EXECUTABLE)
	add_custom_target(update
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash origin main
	)
	set_target_properties(update PROPERTIES FOLDER "util")
endif()

#find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#elseif(NOT MSVC)
#   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

if (Boost_FOUND)
	if(NOT TARGET boost)
		add_custom_target(boost)
		set_target_properties(boost PROPERTIES FOLDER "util")
	endif()
elseif(OPENMIND_BUILD_BOOST)
    message("Required boost components ${BOOST_USED_COMPONENTS}")
    foreach(_module ${BOOST_USED_COMPONENTS})
        if(_module STREQUAL "unit_test_framework"
            OR _module STREQUAL "test_exec_monitor")
		string(CONCAT BOOST_BUILD_PARAMS "${BOOST_BUILD_PARAMS}" " --with-test")
        else()
            string(CONCAT BOOST_BUILD_PARAMS "${BOOST_BUILD_PARAMS}" " --with-${_module}")
        endif()

        string(TOUPPER ${_module} BOOST_MODULE_NAME_UPCASE)
        add_definitions(-DBOOST_${BOOST_MODULE_NAME_UPCASE}_STATIC_LIB)
    endforeach()
    string(CONCAT BOOST_BUILD_PARAMS "${BOOST_BUILD_PARAMS}" " --with-test")
    message("BOOST_BUILD_PARAMS: ${BOOST_BUILD_PARAMS}")

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(Boost_address_model 64)
    else()
      set(Boost_address_model 32)
    endif()

    set(BOOST_BUILD_COMMAND "b2 address-model=${Boost_address_model} runtime-link=static,shared ${BOOST_BUILD_PARAMS}")
    if(WIN32)
        set(ENV{VS150COMNTOOLS} "C:/Program Files/Microsoft Visual Studio/Preview/Community/Common7/Tools")
        separate_arguments(BOOST_EXT_CONFIGURE WINDOWS_COMMAND "bootstrap.bat msvc")
        separate_arguments(BOOST_BUILD_COMMAND WINDOWS_COMMAND "${BOOST_BUILD_COMMAND} install")
    else()
        set(BOOST_EXT_CONFIGURE ./bootstrap.sh)
		if(APPLE)
#			separate_arguments(BOOST_EXT_CONFIGURE UNIX_COMMAND "${BOOST_EXT_CONFIGURE} --prefix=/Library/Developer")
#			set(BOOST_BUILD_COMMAND "${BOOST_BUILD_COMMAND} install")
		endif()
        separate_arguments(BOOST_BUILD_COMMAND UNIX_COMMAND "./${BOOST_BUILD_COMMAND}")
        separate_arguments(BOOST_INSTALL_COMMAND UNIX_COMMAND "./${BOOST_BUILD_COMMAND} install --prefix='${CMAKE_BINARY_DIR}'")
    endif()

    message("ExternalProject_Add: ${BOOST_EXT_CONFIGURE} && ${BOOST_BUILD_COMMAND}")
    ExternalProject_Add(boost GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-${OPENMIND_REQUIRED_BOOST_VERSION}
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ${BOOST_EXT_CONFIGURE}
        BUILD_COMMAND ${BOOST_BUILD_COMMAND}
        INSTALL_COMMAND ${BOOST_INSTALL_COMMAND}
        UPDATE_COMMAND ""
        )
    set(EXTERNAL_FETCHED_BOOST ${PROJECT_BINARY_DIR}/boost-prefix/src/boost CACHE PATH "Path" FORCE)
    set(Boost_INCLUDE_DIR ${EXTERNAL_FETCHED_BOOST} CACHE PATH "Path" FORCE)
    build_command(BOOST_BUILD_CMD TARGET boost)
    message("EXTERNAL_FETCHED_BOOST: ${EXTERNAL_FETCHED_BOOST}")
    message("BOOST_BUILD_CMD ${BOOST_BUILD_CMD}")
    execute_process(COMMAND ${BOOST_BUILD_CMD}
        WORKING_DIRECTORY ${EXTERNAL_FETCHED_BOOST}
        )
    if(WIN32)
        find_package(Boost ${OPENMIND_REQUIRED_BOOST_VERSION}
            PATHS C:/Boost
            COMPONENTS ${BOOST_USED_COMPONENTS}
            )
    endif()
    #find_package(Boost COMPONENTS ${BOOST_USED_COMPONENTS})
    if(Boost_FOUND)
		if(NOT TARGET boost)
			add_custom_target(boost)
		endif()
    endif()
else()
	if(NOT TARGET boost)
		add_custom_target(boost)
	endif()
endif()

set(BOOST_LINK_LIBS
    Boost::serialization
    Boost::system
    Boost::filesystem
    )
if(NOT MSVC OR Boost_VERSION VERSION_LESS 1.80.0)
    set(BOOST_LINK_LIBS ${BOOST_LINK_LIBS}
        Boost::thread)
endif()
set(BOOST_TEST_LINK_LIBS
    ${BOOST_LINK_LIBS}
    Boost::test_exec_monitor
	Boost::unit_test_framework
    )

IF (Boost_FOUND)
    message("Boost_FOUND")
    message("Boost_DIR: ${Boost_DIR}")
    message("Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
    message("EXTERNAL_FETCHED_BOOST: ${EXTERNAL_FETCHED_BOOST}")
    message("Boost_LIBRARY_DIR: ${Boost_LIBRARY_DIR}")
    message("Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
ENDIF()

if (OPENMIND_BUILD_TESTS)
    enable_testing()
	if(NOT MSVC)
		if(NOT APPLE)
    		set(TEST_DEPS "pthread")
		endif()
	endif()
endif ()

macro(find_local_dep)
	message("find local dep: ${ARGN}")
	if(${ARGN} MATCHES " ")
		string(REPLACE " " ";" ARGN ${ARGN})
	endif()
	foreach(dep ${ARGN})
		if(NOT ${dep}_FOUND)
			find_package(${dep})
			if(NOT ${dep}_FOUND)
				find_package(${dep}	HINTS ${CMAKE_INSTALL_PREFIX})
			endif()
			message("${dep}_FOUND: ${${dep}_FOUND}")
		endif()
	endforeach()
endmacro()

macro(dep_find_package)
	message("dep_find_package: searching for ${dep}")
	find_local_dep(${dep})
	if(NOT ${dep}_FOUND)
		string(REPLACE "-" "" dashless ${dep})
		if(NOT dashless STREQUAL dep)
			find_local_dep(${dashless})
		endif()
		if(NOT ${dashless}_FOUND)
			string(REPLACE "-" "_" underscored ${dep})
			set(underscored ${dashless})
			if(NOT dashless STREQUAL underscored)
				find_local_dep(${underscored})
			endif()

			if(NOT ${underscored}_FOUND)
				string(TOLOWER ${underscored} underscored_down)
				if(NOT underscored_down STREQUAL underscored)
					find_local_dep(${underscored_down})
				endif()
				if(NOT ${underscored_down}_FOUND)
					string(TOUPPER ${underscored} underscored_up)
					if(NOT dashless STREQUAL underscored_up)
						find_local_dep(${underscored_up})
					endif()
				endif()
			endif()
		endif()
		message("findpackaged")
		message("dashless: ${dashless}_FOUND: ${${dashless}_FOUND}")
		message("underscored: ${underscored}_FOUND: ${${underscored}_FOUND}")
		message("underscored_up: ${underscored_up}_FOUND: ${${underscored_up}_FOUND}")
		message("underscored_down: ${underscored_down}_FOUND: ${${underscored_down}_FOUND}")
		if(${dashless}_FOUND)
			message("dashless: ${dashless}_FOUND: ${${dashless}_FOUND}")
			set(dep ${dashless})
		elseif(${underscored}_FOUND)
			message("underscored: ${underscored}_FOUND: ${${underscored}_FOUND}")
			set(dep ${underscored})
		elseif(${underscored_up}_FOUND)
			message("underscored_up: ${underscored_up}_FOUND: ${${underscored_up}_FOUND}")
			set(dep ${underscored_up})
		elseif(${underscored_down}_FOUND)
			message("underscored_down: ${underscored_down}_FOUND: ${${underscored_down}_FOUND}")
			set(dep ${underscored_down})
		elseif(OPENMIND_USE_PKG_CONFIG_DEPS)
			message("pkg_check_modules(${underscored_down} IMPORTED_TARGET ${underscored_down}>=1.0)")
			pkg_check_modules(PKG IMPORTED_TARGET ${underscored_down}>=1.0)
			message("PKG_FOUND: ${PKG_FOUND}")
			if(PKG_FOUND)
				message("PKG_CFLAGS: ${PKG_CFLAGS}")
				message("PKG_LIBRARIES: ${PKG_LIBRARIES}")
				pkg_check_modules(${underscored_down} IMPORTED_TARGET ${underscored_down}>=1.0)
				target_link_libraries(${this_target} PUBLIC PkgConfig::${underscored_down})
			else()
				message("Neither find_package nor pkg_config found ${dep}")
			endif()
		endif()
	endif()

	if(${dep}_FOUND)
		message("dep: ${dep}_FOUND: ${${dep}_FOUND}")
		if(${dep}_INCLUDE_DIR)
			message("${dep}_INCLUDE_DIR: ${${dep}_INCLUDE_DIR}")
			target_include_directories(${this_target} PUBLIC ${${dep}_INCLUDE_DIR})
		endif()
		if(${dep}_INCLUDE_DIRS)
			message("${dep}_INCLUDE_DIRS: ${${dep}_INCLUDE_DIRS}")
			target_include_directories(${this_target} PUBLIC ${${dep}_INCLUDE_DIRS})
		endif()
		if(${dep}_LIBRARY)
			target_link_libraries(${this_target} PUBLIC ${${dep}_LIBRARY})
		elseif(NOT ${dep}_LIBRARIES)
			find_library(${dep}_LIBRARIES NAMES ${dep} lib${dep} ${dep}lib lib${dep}lib)
		endif()
		message("${dep}_LIBRARIES: ${${dep}_LIBRARIES}")
		if(${dep}_LIBRARIES)
			foreach(lib ${${dep}_LIBRARIES})
				find_library(libfound ${lib})
				if(libfound OR EXISTS "${lib}")
					set(repo SYSTEM)
					break()	
				endif()
				message("lib ${lib}")
			endforeach()
			message("repo ${repo}")
			if(repo STREQUAL "SYSTEM")
				message("Found ${dep}_LIBRARIES: ${${dep}_LIBRARIES}")
				target_link_libraries(${this_target} PUBLIC ${${dep}_LIBRARIES})
			else()
				message("Found no ${dep}_LIBRARIES: ${${dep}_LIBRARIES}")
				ext(${dep} ${repo})
				if(OPENMIND_BUILD_BOOST)
					add_dependencies(${dep} boost)
				endif()
			endif()
		endif()		
	else()
		message("No ${dep} local installations found")
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
			message("dep ${dep} target is ready")
			add_dependencies(${this_target} ${dep})
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
				if(OPENMIND_BUILD_BOOST)
					add_dependencies(${dep} boost)
				endif()
				message("Dependency ${dep} took from ${repo}")
				if(dep MATCHES [qQ]t)
					set(OPENMIND_USE_QT ON CACHE BOOL "Qt used is enabled for dependency ${dep}" FORCE)
				endif()
			endif()
		elseif(dep MATCHES /)
			dep("https://github.com/${dep}.git")
		else()
			message("Assuming the ${dep} is preinstalled")
			dep_find_package()
		endif()
	endforeach()
endmacro()

macro(deps)
    string(STRIP "${ARGN}" dependencies_stripped)
	string(REPLACE " " ";" dependencies_listified "${dependencies_stripped}")
    foreach(tg ${dependencies_listified})
      dep(${tg})
    endforeach()
endmacro()

function(apply_target_commons this_target)
	string(REPLACE "-" "" dashless ${this_target})
	string(TOUPPER ${dashless} this_target_up)
	target_compile_definitions(${this_target} PUBLIC
		HAS_BOOST
		BOOST_SYSTEM_NO_DEPRECATED
		BOOST_ERROR_CODE_HEADER_ONLY
		BOOST_COMPUTE_USE_CPP11
		-DSRC_DIR="${CMAKE_SOURCE_DIR}/"
		-D_${dashless}_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
		-D_${this_target_up}_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
		)
	if(NOT OPENMIND_DEBUG_CHECKS)
		target_compile_definitions(${this_target} PUBLIC NOOMDEBUG)
	endif()
	set_target_properties(${this_target} PROPERTIES
		CXX_STANDARD 20
		)
	if(MSVC)
		target_compile_definitions(${this_target} PUBLIC
			APPLE_CONSTEXPR=
			MSVC_CONSTEXPR=constexpr
			NO_MSVC_CONSTEXPR=
			MSVC
			)
		target_compile_options(${this_target} PUBLIC
			/bigobj
			/FS
			/constexpr:steps1000000000
			/source-charset:utf-8
			"$<$<CONFIG:DEBUG>:/ZI>"
			)
	    target_link_directories(${this_target} PUBLIC
			${Boost_INCLUDE_DIR}/stage/lib
			${EXTERNAL_FETCHED_BOOST}/stage/lib
			C:/Boost/lib
			)
	else()
		target_compile_definitions(${this_target} PUBLIC
			MSVC_CONSTEXPR=
			NO_MSVC_CONSTEXPR=constexpr
			)
		if(APPLE)
			target_compile_definitions(${this_target} PUBLIC
				APPLE_CONSTEXPR=constexpr
				)
		else()
			target_compile_definitions(${this_target} PUBLIC
				APPLE_CONSTEXPR=
				)
		endif()
	endif()

	if(Boost_INCLUDE_DIR)
		target_include_directories(${this_target} PUBLIC ${Boost_INCLUDE_DIR})
	endif()
	if(Boost_INCLUDE_DIRS)
		target_include_directories(${this_target} PUBLIC ${Boost_INCLUDE_DIRS})
	endif()
	if(EXTERNAL_FETCHED_BOOST)
		target_include_directories(${this_target} PUBLIC ${EXTERNAL_FETCHED_BOOST})
	endif()

    target_link_directories(${this_target} PUBLIC
		${Boost_LIBRARY_DIRS}
		)

	get_target_property(target_type ${this_target} TYPE)
	if (target_type STREQUAL "EXECUTABLE")
		
	endif ()
		
endfunction(apply_target_commons)

function(test)
    string(STRIP "${ARGN}" test_libs)
	get_filename_component(this_target ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
	get_filename_component(this_target ${this_target} NAME)
	project(${this_target})
	message("\nCreating Tests for ${this_target}")
	file(GLOB src *.cpp)
	message("test_libs ${test_libs}")
	set(libs ${test_libs} ${TEST_DEPS})#pthread
	if(NOT MSVC AND Boost_FOUND)
		set(libs ${libs} ${BOOST_TEST_LINK_LIBS})
	endif()
    if(OPENMIND_USE_QT)
		qtect()
    endif()
	message("libs ${libs}")
	foreach(tg ${libs})
		message("${tg}")
		if(TARGET ${tg})
			get_target_property(type ${tg} TYPE)
			message("tg type ${type}")
			if(${type} STREQUAL UTILITY)
				set(dep_on_targets ${dep_on_targets} ${tg})
				list(REMOVE_ITEM libs ${tg})
				list(APPEND libs ${${tg}_LIBRARIES})
				message("removed ${tg} from libs: ${libs}")
			endif()
    	endif()
    endforeach()
	message("targets ${dep_on_targets}")
	foreach(TEST ${src})
	    get_filename_component(TEST_NAME ${TEST} NAME_WE)
	    message("test ${TEST_NAME}")
	    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
		apply_target_commons(${TEST_NAME})
		set_target_properties(${TEST_NAME} PROPERTIES
			FOLDER "test"
		)
		target_compile_definitions(${TEST_NAME} PUBLIC
			-DTEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
			-DTEST_BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}/"
			)
		add_dependencies(${TEST_NAME} prerequisites ${this_target})
		target_link_directories(${TEST_NAME} PUBLIC
			/usr/local/lib
			${Boost_INCLUDE_DIR}/stage/lib
			${Boost_INCLUDE_DIR}/../../lib
			${EXTERNAL_FETCHED_BOOST}/stage/lib
			${EXTERNAL_FETCHED_BOOST}/../../lib
			${CMAKE_BINARY_DIR}/lib
			${CMAKE_BINARY_DIR}/lib64
			)
        target_link_libraries(${TEST_NAME} PUBLIC ${this_target})
        foreach(ll ${libs})
            string(STRIP ${ll} thelib)
			message("link ${thelib}")
            target_link_libraries(${TEST_NAME} PUBLIC ${thelib})
			if(${thelib}_INCLUDE_DIR)
				message("include ${${thelib}_INCLUDE_DIR}")
				target_include_directories(${TEST_NAME} PUBLIC ${${thelib}_INCLUDE_DIR})
			endif()
        endforeach()
        foreach(tg ${libs})
            if(TARGET ${tg})
                add_dependencies(${TEST_NAME} ${tg})
            endif()
        endforeach()

        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.db)
        add_test(NAME ${TEST_NAME}
            COMMAND $<TARGET_FILE:${TEST_NAME}>
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.db
	    )
	    if(NOT "${dep_on_targets}" STREQUAL "")
	    	message("making ${TEST_NAME} depend on ${dep_on_targets}")
			add_dependencies(${TEST_NAME} ${this_target} ${dep_on_targets})
		endif()
	endforeach()
endfunction()

macro(ext name location)
	message("External ${name} ${location}")
	if(NOT TARGET ${name})
		if(NOT OPENMIND_UPDATE_EXTERNAL_DEPS)
			set(OPENMIND_UPDATE_EXTERNAL_DEPS_CMD_PARAM UPDATE_COMMAND "")
		endif()
		if(MSVC)
			set(build_type -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}) # overcoming runtime mismatch issue
		else()
			set(build_type -DCMAKE_BUILD_TYPE=Release)
		endif()
		string(TOUPPER ${name} _name_upcase)
		set(ARGS
			${build_type}
			-D${name}_BUILD_TESTS=OFF
			-D${_name_upcase}_BUILD_TESTS=OFF
			-D${name}_BUILD_BENCHMARKS=OFF
			-D${_name_upcase}_BUILD_BENCHMARKS=OFF
			-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
			-DCMAKE_AR=${CMAKE_AR}
			-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
			-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
			-DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}
			-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
			-DGIT_EXECUTABLE=${GIT_EXECUTABLE}
			-DHAVE_STD_REGEX=ON
			-DRUN_HAVE_STD_REGEX=1
			)
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

		execute_process(
			COMMAND ${GIT_EXECUTABLE} ls-remote ${location} tags/*
			RESULT_VARIABLE RESULT
			OUTPUT_VARIABLE OUTPUT)

		set (DEFAULT_TAG HEAD)
		set (${name}_TAG ${DEFAULT_TAG} CACHE STRING "Select a repo tag")

		string (REGEX MATCHALL "([^/]+)[\^{}]\n" TAGS ${OUTPUT})
		string (REGEX REPLACE "\n" ";"  TAGS ${TAGS})
		set_property (CACHE ${name}_TAG PROPERTY STRINGS ${TAGS})

		ExternalProject_Add(${name}
			GIT_REPOSITORY ${location}
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
endmacro()

macro(lib)
    string(STRIP "${ARGN}" deps)
    get_filename_component(this_target ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    set(${this_target}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE FILEPATH "${this_target} include path")
    project(${this_target})
    message("\nCreating Library: ${this_target}")
    file(GLOB headers *.h *.hpp *.inc)
    file(GLOB src *.cpp *.c *.CC *.cc)
    add_library(${this_target} ${src} ${headers})
	APPLY_TARGET_COMMONS(${this_target})
	if(CMAKE_CXX_STANDARD)
		set_target_properties(${this_target} PROPERTIES CXX_STANDARD ${CMAKE_CXX_STANDARD})
	endif(CMAKE_CXX_STANDARD)
    set_target_properties(${this_target} PROPERTIES
		FOLDER "libs"
		PUBLIC_HEADER "${headers}"
		)	
    target_include_directories(${this_target} PUBLIC
        ${OPENMIND_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${${this_target}_INCLUDE_DIR}
        )
    add_dependencies(${this_target} prerequisites)
	message("add_library(${this_target} ${src})")
	if(NOT MSVC AND Boost_FOUND)
		target_link_libraries(${this_target} PUBLIC ${BOOST_LINK_LIBS})
		foreach (boostcomponent ${BOOST_ADDITIONAL_COMPONENTS})
			string(TOUPPER ${boostcomponent} boostcomponent)
			target_link_libraries(${this_target} PUBLIC ${Boost_${boostcomponent}_LIBRARY})
		endforeach ()
	endif()
	#message("target_link_libraries(${this_target} PUBLIC ${deps})")
    deps(${TEST_DEPS} ${deps})
	set(this_target_libs "")
	message("foreaching ${deps}")
	foreach(tg ${deps})
		if(TARGET ${tg})
			get_target_property(this_target_type ${tg} TYPE)
			if(NOT this_target_type STREQUAL UTILITY)
				message("dep on target ${tg}")
				add_dependencies(${this_target} ${tg})
				list(APPEND this_target_libs ${tg})
			endif()
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
				add_dependencies(${this_target} ${t})
				if(NOT this_target_type STREQUAL UTILITY)
					list(APPEND this_target_libs ${t})
				elseif(${t}_LIBRARIES)
					list(APPEND this_target_libs ${${t}_LIBRARIES})
				endif()
			elseif(${t} MATCHES "^lib.+")
				message("its lib")
				STRING(REGEX REPLACE "^lib(.+)" "\\1"  tg ${t})
				message("${t} -> ${tg}")
				if(TARGET ${tg})
					get_target_property(this_target_type ${tg} TYPE)
					if(NOT this_target_type STREQUAL UTILITY)
						message("dep on target ${tg}")
						add_dependencies(${this_target} ${tg})
						list(APPEND this_target_libs ${tg})
					endif()
				endif()
			elseif(NOT this_target_type STREQUAL UTILITY)
				list(APPEND this_target_libs ${t})
			endif()
        endif()
    endforeach()

    set(${this_target}_LIBRARIES ${this_target_libs} CACHE FILEPATH "${this_target} libs to link")
    message("target_link_directories(${this_target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})")
    target_link_directories(${this_target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    if(this_target_libs AND NOT this_target_libs STREQUAL "")
        message("${this_target} libs: ${this_target_libs}")
        target_link_libraries(${this_target} PUBLIC ${this_target_libs})
    else()
        message("${this_target} has no libs")
    endif()
    if(MSVC)
        target_compile_options(${this_target} PUBLIC /constexpr:steps1000000000>)
    else()
        target_link_libraries(${this_target} PUBLIC
                        pthread
                        )
    endif()

    install(TARGETS ${this_target})

    if (OPENMIND_BUILD_TESTS OR BUILD_TESTS)
		if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
        	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
    	elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Test)
        	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Test)
		elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)
        	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
		elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Tests)
        	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Tests)
		endif ()
	endif ()

    if (OPENMIND_BUILD_3RD_PARTY_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyTests)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyTests)
    endif ()

endmacro()

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
    foreach(item ${first})
        set(item ${CMAKE_CURRENT_SOURCE_DIR}/${item})
        IF(IS_DIRECTORY ${item} AND EXISTS ${item}/CMakeLists.txt)
            message("${item} is dir with CMakeLists.txt")
            get_filename_component(name ${item} NAME)
            add_subdirectory(${item})
            list(REMOVE_ITEM items ${item})
        endif()
    endforeach()

    foreach(item ${items})
        IF(IS_DIRECTORY ${item} AND EXISTS ${item}/CMakeLists.txt)
            message("${item} is dir with CMakeLists.txt")
            get_filename_component(name ${item} NAME)
            if(NOT "${name}" STREQUAL "OpenMind"
                AND NOT "${name}" STREQUAL "omnn"
                ) # already included first
                add_subdirectory(${item})
            endif()
        endif()
    endforeach()
endmacro()

macro(exe)
    string(STRIP "${ARGN}" deps)
    get_filename_component(this_target ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    project(${this_target})
    message("\nCreating Executable: ${this_target}")
    file(GLOB src *.cpp *.h)
    file(GLOB qmlsrc *.qml)
    file(GLOB qtsrc *.qrc)
	if(qtsrc OR qmlsrc)
		set(OPENMIND_USE_QT ON CACHE BOOL "Qt used is enabled for dependency ${dep}" FORCE)
	endif()
    if(OPENMIND_USE_QT)
		qtect()
    endif()
    if(Qt${QT_VERSION_MAJOR}_FOUND)
        if(QT_VERSION_MAJOR EQUAL 6)
            qt_standard_project_setup()
        endif()
        qt_add_resources(qml_QRC ${qtsrc})
        list(APPEND src ${qml_QRC})
        qt_add_executable(${this_target} MANUAL_FINALIZATION ${src})
        target_compile_definitions(${this_target} PUBLIC $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
        if(NOT qtsrc)
            if(NOT VERSION)
                set(VERSION 0.18) #TODO: deduce from git branch name/history
            endif()
            foreach(qmlfile ${qmlsrc})
				message("Adding QML ${qmlfile}")
                qt_add_qml_module(app${this_target} URI ${this_target} VERSION ${VERSION} QML_FILES ${qmlfile})
            endforeach()
        endif()
        foreach(component ${QT_COMPONENTS})
            message("Qt${QT_VERSION_MAJOR}${component}_INCLUDE_DIRS: ${Qt${QT_VERSION_MAJOR}${component}_INCLUDE_DIRS}")
            target_include_directories(${this_target} PUBLIC
                ${Qt${QT_VERSION_MAJOR}${component}_INCLUDE_DIRS})
            target_link_libraries(${this_target} PUBLIC
                Qt${QT_VERSION_MAJOR}::${component})
        endforeach()

        if(QT_VERSION_MAJOR EQUAL 6)
            qt_import_qml_plugins(${this_target})
            qt_finalize_executable(${this_target})
        endif()
    elseif(ANDROID)
        add_library(${this_target} SHARED ${src})
    else()
        add_executable(${this_target} ${src})
    endif()
    APPLY_TARGET_COMMONS(${this_target})
    add_dependencies(${this_target} prerequisites)
    if(Qt${QT_VERSION_MAJOR}_FOUND)

        if(APPLE AND PROJECT_VERSION)
			set_target_properties(${this_target} PROPERTIES
				MACOSX_BUNDLE_GUI_IDENTIFIER ${this_target}.deduction-fw.org
				MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
				MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
			)
        endif()
        set_target_properties(${this_target} PROPERTIES
            MACOSX_BUNDLE TRUE
            WIN32_EXECUTABLE TRUE
        )
        install(TARGETS ${this_target} BUNDLE DESTINATION .)

    else()
        install(TARGETS ${this_target})
    endif()

    target_include_directories(${this_target} PUBLIC
        ${OPENMIND_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}/include
        ${${this_target}_INCLUDE_DIR}
        )
    target_link_directories(${this_target} PUBLIC
        ${CMAKE_BINARY_DIR}/lib
        ${Boost_LIBRARIES}
        ${Boost_INCLUDE_DIR}/stage/lib
        ${Boost_INCLUDE_DIR}/../../lib
        /usr/local/lib
        )

    foreach(item ${deps})
        message("adding ${item} sublib ${LIB} into ${this_target}")
        if(TARGET ${item})
            message("Linking target ${item}")
            target_link_libraries(${this_target} PUBLIC ${item})
            if(${item}_LIBRARIES)
                message("Linking ${item}_LIBRARIES ${${item}_LIBRARIES}")
                target_link_libraries(${this_target} PUBLIC ${${item}_LIBRARIES})
                if(NOT "${${item}_LIBRARIES}" MATCHES ${item})
                    message(WARNING "${item}_LIBRARIES '${${item}_LIBRARIES}' has no ${item}")
                endif()
            endif()
        elseif(${item}_LIBRARIES)
            message("Linking ${item}_LIBRARIES ${${item}_LIBRARIES}")
            target_link_libraries(${this_target} PUBLIC ${${item}_LIBRARIES})
            if(NOT ${${item}_LIBRARIES} MATCHES ${item})
                message(WARNING "${item}_LIBRARIES '${${item}_LIBRARIES}' has no ${item}")
            endif()
        elseif(${item}_LIBRARY)
            target_link_libraries(${this_target} PUBLIC ${${item}_LIBRARY})
        else()
            dep(${item})
        endif()

        if(${item}_INCLUDE_DIR)
            message("adding include ${${item}_INCLUDE_DIR} into ${this_target} for dependency ${item}")
            if(${item}_INCLUDE_DIR)
                target_include_directories(${this_target} PUBLIC ${${item}_INCLUDE_DIR})
            endif()
            if(${item}_INCLUDE_DIRS)
                target_include_directories(${this_target} PUBLIC ${${item}_INCLUDE_DIRS})
            endif()
        endif()
    endforeach()
    if(NOT MSVC)
        target_link_libraries(${this_target} PUBLIC
            pthread
            ${BOOST_LINK_LIBS}
            )
    endif()

    if (OPENMIND_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
    elseif(OPENMIND_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Test)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Test)
    endif ()

endmacro()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib
                    ${CMAKE_BINARY_DIR}/lib64
                    )

fold(
    omnn
    OpenMind
)

build_command(PREREQUISITES_BUILD_CMD
	TARGET prerequisites
	PARALLEL_LEVEL ${CMAKE_AUTOGEN_PARALLEL}
)
message("Building target prerequisites in ${CMAKE_BINARY_DIR} for prerequisites:")
execute_process(COMMAND ${PREREQUISITES_BUILD_CMD}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMAND_ECHO STDOUT 
)
