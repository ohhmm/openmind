cmake_minimum_required(VERSION 3.0)
set(PRODUCT_NAME "OpenMind")
project(${PRODUCT_NAME})
enable_language(CXX)
set (CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(_IS_DEBUG TRUE)
else()
	set(_IS_DEBUG FALSE)
endif()

option(OPENMIND_BUILD_BOOST "Fetch&build boost" ${MSVC})
option(OPENMIND_BUILD_SAMPLES "Build various samples" FALSE)
option(OPENMIND_BUILD_TESTS "Build various TESTS" ${_IS_DEBUG})
option(OPENMIND_BUILD_3RD_PARTY_TESTS "Build various 3rd party TESTS to compare to openmind by performance" FALSE)
option(OPENMIND_UPDATE_EXTERNAL_DEPS "Update ExternalProject dependencies" FALSE)
option(OPENMIND_USE_OPENCL "OpenCL GPU calculations" FALSE)

include(ExternalData)
include(ExternalProject)

add_definitions(
    -DBOOST_SYSTEM_NO_DEPRECATED
    -DBOOST_ERROR_CODE_HEADER_ONLY
)
if(MSVC)
    add_definitions(/source-charset:utf-8) #include "pi.h"
endif()

include_directories(${CMAKE_SOURCE_DIR}
#    ${CMAKE_SOURCE_DIR}/boost/gil/include
    ${CMAKE_SOURCE_DIR}/boost/multiprecision/include
)

find_package(git)

#find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#elseif(NOT MSVC)
#   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

if(NOT OPENMIND_BUILD_BOOST)
    find_package(Boost 1.71)
endif()
if (Boost_FOUND)
    add_custom_target(boost)
else()
    set(BOOST_USED_COMPONENTS system serialization date_time thread)
    if (${OPENMIND_BUILD_TESTS})
        set(BOOST_USED_COMPONENTS ${BOOST_USED_COMPONENTS} test)
    endif ()
    message("Finding boost components ${BOOST_USED_COMPONENTS}")
    foreach(_module ${BOOST_USED_COMPONENTS})
        message("${BOOST_BUILD_PARAMS}")
        string(CONCAT BOOST_BUILD_PARAMS "${BOOST_BUILD_PARAMS}" " --with-${_module}")
        message("${BOOST_BUILD_PARAMS}")
    endforeach()

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(Boost_address_model 64)
    else()
      set(Boost_address_model 32)
    endif()

    if(WIN32)
        set(ENV{VS150COMNTOOLS} "C:/Program Files/Microsoft Visual Studio/Preview/Community/Common7/Tools")
        set(BOOST_EXT_CONFIGURE "bootstrap.bat msvc")
        set(BOOST_BUILD_COMMAND "b2 address-model=${Boost_address_model} ${BOOST_BUILD_PARAMS}")
    else()
        set(BOOST_EXT_CONFIGURE ./bootstrap.sh)
        separate_arguments(BOOST_BUILD_COMMAND UNIX_COMMAND "./b2 address-model=${Boost_address_model} ${BOOST_BUILD_PARAMS}")
    endif()
    message("ExternalProject_Add ${BOOST_EXT_CONFIGURE} && ${BOOST_BUILD_COMMAND}")
    if(_IS_DEBUG)
        ExternalProject_Add(boost
            GIT_REPOSITORY https://github.com/boostorg/boost.git
            GIT_TAG boost-1.72.0
            GIT_SUBMODULES ${BOOST_USED_COMPONENTS}
            BUILD_IN_SOURCE 1
            LOG_CONFIGURE 1
            CONFIGURE_COMMAND ${BOOST_EXT_CONFIGURE}
            BUILD_COMMAND ${BOOST_BUILD_COMMAND} headers
            INSTALL_COMMAND ""
            )
    else()
        ExternalProject_Add(boost GIT_REPOSITORY https://github.com/boostorg/boost.git
            BUILD_IN_SOURCE 1
            CONFIGURE_COMMAND ${BOOST_EXT_CONFIGURE}
            BUILD_COMMAND ${BOOST_BUILD_COMMAND}
            INSTALL_COMMAND ""
            UPDATE_COMMAND ""
            )
    endif()
    set(Boost_INCLUDE_DIR ${PROJECT_BINARY_DIR}/boost-prefix/src/boost CACHE PATH "Path" FORCE)
    build_command(BOOST_BUILD_CMD TARGET boost)
    message(BOOST_BUILD_CMD ${BOOST_BUILD_CMD})
    #execute_process(COMMAND ${BOOST_BUILD_CMD}
        #WORKING_DIRECTORY ${Boost_INCLUDE_DIR}
        #)
    #find_package(Boost COMPONENTS ${BOOST_USED_COMPONENTS})
    if(NOT ${Boost_FOUND})
        add_custom_target(conf ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR} DEPENDS boost)
    else()
        add_custom_target(conf)
    endif()
endif()

function(add_library targetName)
    _add_library(${targetName} ${ARGN})
    add_dependencies(${targetName} boost) 
endfunction()

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
IF (Boost_FOUND)
    message("Boost_FOUND")
    ADD_DEFINITIONS( "-DHAS_BOOST" )
    link_directories(${Boost_LIBRARY_DIR})
#    if(MSVC)
#	    link_directories(${Boost_INCLUDE_DIR}/stage/lib)
#    endif()
ENDIF()

if(OPENMIND_USE_OPENCL)
	find_package(opencl)
	include_directories(${OPENCL_INCLUDE_DIR})
	add_definitions(-DOPENMIND_USE_OPENCL)
endif()

if (${OPENMIND_BUILD_TESTS})
    enable_testing()
	if(NOT MSVC)
		if(NOT APPLE)
    		set(TEST_DEPS "pthread")
		endif()
	endif()
endif ()

macro(lib)
    string(STRIP "${ARGN}" deps)
    get_filename_component(this_target ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    set(${this_target}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE FILEPATH "${this_target} include path")
    project(${this_target})
	message("\nCreating Library: ${this_target}")
    file(GLOB src *.cpp *.h)
    add_library(${this_target} ${src})
	message("add_library(${this_target} ${src})")
    target_link_libraries(${this_target} ${deps})
	message("target_link_libraries(${this_target} ${deps})")
    link_directories(${CMAKE_CURRENT_SOURCE_DIR})
	message("link_directories(${CMAKE_CURRENT_SOURCE_DIR})")
    set(TEST_DEPS "${TEST_DEPS} ${deps}")
	set(this_target_libs ${this_target})
    foreach(tg ${deps})
		message("foreaching ${tg}")
        if(TARGET ${tg})
			message("dep on target ${tg}")
            add_dependencies(${this_target} ${tg})
			list(APPEND this_target_libs ${tg})
		else()
			get_filename_component(t ${tg} NAME_WLE)
			message("its ${t}")
			if(TARGET ${t})
				message("dep on target ${t}")
				add_dependencies(${this_target} ${t})
				list(APPEND this_target_libs ${t})
			elseif(${t} MATCHES "^lib.+")
				message("its lib")
				STRING(REGEX REPLACE "^lib(.+)" "\\1"  tg ${t})
				message("${t} -> ${tg}")
				if(TARGET ${tg})
					message("dep on target ${tg}")
					add_dependencies(${this_target} ${tg})
					list(APPEND this_target_libs ${tg})
				endif()
			else()
				list(APPEND this_target_libs ${t})
			endif()
        endif()
    endforeach()
	set(${this_target}_LIBRARIES ${this_target_libs} CACHE FILEPATH "${this_target} libs to link")
	message("${this_target} libs: ${this_target_libs}")
endmacro()

function(test)
    string(STRIP "${ARGN}" test_libs)
	get_filename_component(this_target ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
	get_filename_component(this_target ${this_target} NAME)
	project(${this_target})
	message("\nCreating Tests for ${this_target}")
	file(GLOB src *.cpp)
	add_definitions(
	    -DTEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
	    -DTEST_BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}/"
	    )
	link_directories(
	    ${Boost_LIBRARIES}
	    /usr/local/lib
	    ${Boost_INCLUDE_DIR}/stage/lib
	    )
	message("test_libs ${test_libs}")
	set(libs ${test_libs} ${OpenCL_LIBRARY} ${TEST_DEPS})#pthread
	if(NOT MSVC)
		set(libs ${libs} boost_test_exec_monitor boost_thread boost_serialization boost_system)
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
	    add_executable(${TEST_NAME} ${TEST_NAME})
        foreach(ll ${libs})
            string(STRIP ${ll} thelib)
            target_link_libraries(${TEST_NAME} ${thelib})
        endforeach()
        foreach(tg ${libs})
            if(TARGET ${tg})
                add_dependencies(${TEST_NAME} ${tg})
            endif()
        endforeach()
	    add_test(NAME ${TEST_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME})
	    if(NOT ${dep_on_targets} STREQUAL "")
	    	message("making ${TEST_NAME} depend on ${dep_on_targets}")
			add_dependencies(${TEST_NAME} ${dep_on_targets})
		endif()
	endforeach()
endfunction()

include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/lib)

macro(ext name location)
    if(OPENMIND_UPDATE_EXTERNAL_DEPS)
    	ExternalProject_Add(${name}
			GIT_REPOSITORY ${location}
			BUILD_IN_SOURCE 1
			INSTALL_DIR ${CMAKE_BINARY_DIR}
			CMAKE_ARGS
	            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
			)
    else()
        	ExternalProject_Add(${name}
			GIT_REPOSITORY ${location}
			BUILD_IN_SOURCE 1
			INSTALL_DIR ${CMAKE_BINARY_DIR}
			UPDATE_COMMAND ""
			CMAKE_ARGS
	            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
			)
    endif()
	if(TARGET ${name})
		message("${name} is target")
	else()
		message("${name} is not a target")
	endif()

	set(${name}_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include CACHE PATH "Path")
	if(MSVC)
		set(${name}_LIBRARIES ${CMAKE_BINARY_DIR}/lib/${name}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE PATH "Path")
	else()
		set(${name}_LIBRARIES ${CMAKE_BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${name}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE PATH "Path")
	endif()

	include_directories(${${name}_INCLUDE_DIR})
	link_directories(${CMAKE_BINARY_DIR}/lib)
endmacro()

macro(dep)
	set(dep ${ARGN})
	if(dep MATCHES /)
		set(repo "https://github.com/${dep}.git")
		get_filename_component(dep ${ARGN} NAME)
		find_package(${dep})
		if(${dep}_FOUND)
			include_directories(${${dep}_INCLUDE_DIR})
		else()
			ext(${dep} ${repo})
		endif()
	else()
		find_package(${dep})
		if(${dep}_FOUND)
			include_directories(${${dep}_INCLUDE_DIR})
		else()
    		set(repo "https://github.com/lib${dep}/${dep}.git")
			ext(${dep} ${repo})	
    	endif()
	endif()
	message("Dependency ${dep} took from ${repo}")
endmacro()

macro(fold)
	file(GLOB items *)
	foreach(item ${items})
		IF(IS_DIRECTORY ${item})
			if(EXISTS ${item}/CMakeLists.txt)
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
	include_directories(${OpenMind_Incs})
    foreach(item ${deps})
        message("adding include ${${item}_INCLUDE_DIR} into ${this_target} for dependency ${item}")
        include_directories(${${item}_INCLUDE_DIR})
    endforeach()
	link_directories(
			${Boost_LIBRARIES}
			/usr/local/lib
			${Boost_INCLUDE_DIR}/stage/lib
	)
	add_executable(${this_target} ${src})
	target_link_libraries(${this_target} ${deps})
	foreach(item ${deps})
		message("adding ${item} sublib ${LIB} into ${this_target}")
		target_link_libraries(${this_target} ${${item}_LIBRARIES})
	endforeach()
endmacro()

fold()
