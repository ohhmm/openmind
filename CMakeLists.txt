cmake_minimum_required(VERSION 3.14)

get_filename_component(PRODUCT_NAME ${CMAKE_SOURCE_DIR} NAME_WE)
string(REGEX REPLACE "/" "\\\\" PRODUCT_NAME ${PRODUCT_NAME})
project(${PRODUCT_NAME} CXX)

enable_language(CXX)
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message("BIN_DIR=${CMAKE_BINARY_DIR}")
message("SRC_DIR=${CMAKE_SOURCE_DIR}")
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	message("OPENMIND_DIR=${CMAKE_CURRENT_SOURCE_DIR}")
	if(EXISTS "${CMAKE_SOURCE_DIR}/../openmind/CMakeLists.txt")
		get_filename_component(SIBLING_PROJECTS_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
		message("SIBLING_PROJECTS_DIR=${SIBLING_PROJECTS_DIR}")
		if(NOT FETCHCONTENT_SOURCE_DIR_OPENMIND STREQUAL "${SIBLING_PROJECTS_DIR}/openmind")
			message("Updating FETCHCONTENT_SOURCE_DIR_OPENMIND=${FETCHCONTENT_SOURCE_DIR_OPENMIND}")
			set(FETCHCONTENT_SOURCE_DIR_OPENMIND "${SIBLING_PROJECTS_DIR}/openmind" CACHE PATH "OpenMind dirictory path" FORCE)
		endif()
	endif()
	message("FETCHCONTENT_SOURCE_DIR_OPENMIND=${FETCHCONTENT_SOURCE_DIR_OPENMIND}")
endif()
message("PRODUCT_NAME=${PRODUCT_NAME}")
message("CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
message("CMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}")
if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
	EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
	set(CMAKE_OSX_ARCHITECTURES ${ARCHITECTURE} CACHE PATH "May be used for crosscompilation" FORCE)
	message("CMAKE_OSX_ARCHITECTURES=${ARCHITECTURE}")
endif()

if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
	message(WARNING "build dir matches source dir")
endif()

set(OPENMIND_PRIME_TABLE Boost CACHE STRING "Table of predefined prime numbers")
if(CMAKE_BUILD_TYPE)
	if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
		set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} CACHE PATH "hint for find_package")
		set(_IS_DEBUG TRUE)
		if(MSVC)
			add_compile_options($<$<CONFIG:Debug>:/constexpr:steps1000000000>)
			add_link_options(/stack:1000000000)
		else()
			add_definitions(-DBOOST_ALL_DYN_LINK)
		endif()
	else()
		set(_IS_DEBUG FALSE)
		set(Boost_USE_STATIC_LIBS ON)
		SET(BUILD_SHARED_LIBS OFF)
		add_definitions(-DBOOST_ALL_STATIC_LINK)
		if(CMAKE_BUILD_TYPE MATCHES [Rr][Ee][Ll][Ee][Aa][Ss][Ee])
			set(OPENMIND_PRIME_TABLE Bundled CACHE STRING "Table of predefined prime numbers")
			option(OPENMIND_USE_CPACK "Build an installer" ON)
		else()
			if(NOT CMAKE_BUILD_TYPE)
				message(WARNING "Consider setting CMAKE_BUILD_TYPE")
			endif()
		endif()
	endif()
else()
	set(Boost_USE_STATIC_LIBS ${WIN32} CACHE BOOL "Use static libraries" FORCE)
endif()
set(OPENMIND_PRIME_TABLE_OPTIONS Boost Bundled)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(address_model 64)
else()
	set(address_model 32)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/"
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/"
	"${CMAKE_BINARY_DIR}/lib/cmake"
	"${CMAKE_SOURCE_DIR}/cmake"
	)
include(bins)

option(OPENMIND_BUILD_TESTS "Build various TESTS" ${_IS_DEBUG})
option(OPENMIND_DEBUG_CHECKS "Additional debug checks (NOOMDEBUG define)" ${_IS_DEBUG})
option(OPENMIND_DEPEND_ON_PREREQUISITES "Better consider installing prerequisites by building the target 'prerequisites' upon initial setup. This way its not checked each build." OFF)
option(OPENMIND_BUILD_SAMPLES "Build various samples" FALSE)
option(OPENMIND_BUILD_GC "Use garbage collection. Faster but requires abundant memory." OFF)
option(OPENMIND_BUILD_3RD_PARTY_TESTS "Build various 3rd party TESTS to compare to openmind by performance" FALSE)
option(OPENMIND_PRIME_MINING "Extend primes table by appending to Primes.inc source code file during CPU cores idle" ${_IS_DEBUG})
option(OPENMIND_UPDATE_EXTERNAL_DEPS "Update ExternalProject dependencies" FALSE)
option(OPENMIND_USE_QT "Link Qt libs" OFF)

find_package(PkgConfig)
option(OPENMIND_USE_PKG_CONFIG_DEPS "Use pkg-config dependencies" ${PKG_CONFIG_FOUND})
if(PKG_CONFIG_FOUND)
	message("pkg-config may be used to detect some of dependencies")
endif()

find_program(INTEL_COMPILER_PATH icpx)
if(INTEL_COMPILER_PATH)
	message("INTEL_COMPILER_PATH: ${INTEL_COMPILER_PATH}")
	set(CMAKE_CXX_COMPILER ${INTEL_COMPILER_PATH} CACHE STRING "Intel compiler path" FORCE)
	find_package(IntelDPCPP)
	if(IntelDPCPP_FOUND)
		message("IntelDPCPP_FOUND: ${IntelDPCPP_FOUND}")
	endif()
endif()

find_package(CUDAToolkit)
find_package(OpenCL)
option(OPENMIND_USE_OPENCL "Use OpenCL calculations" ${OpenCL_FOUND})

if(NOT WIN32)
	find_package(TBB)
	option(OPENMIND_USE_TBB "Link TBB" ${TBB_FOUND})
	if(OPENMIND_USE_TBB)
		add_definitions(-DOPENMIND_USE_TBB)
	endif()
endif()
set(OPENMIND_PREFERRED_BOOST_VERSION 1.84.0 CACHE STRING "Boost library version to use")
set(OPENMIND_REQUIRED_BOOST_VERSION 1.81.0 CACHE STRING "Boost library minimum compatible version to use")
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
set(_BOOST_USED_COMPONENTS
	${BOOST_USED_COMPONENTS}
	${BOOST_ADDITIONAL_COMPONENTS}
	date_time
	filesystem
	iostreams
	locale
	serialization
	system
	thread
	CACHE STRING "Components" FORCE)
if(OPENMIND_BUILD_TESTS OR _IS_DEBUG OR NOT CMAKE_BUILD_TYPE)
    set(_BOOST_USED_COMPONENTS ${_BOOST_USED_COMPONENTS}
        unit_test_framework
        test_exec_monitor
		CACHE STRING "Components" FORCE)
endif()
list(REMOVE_DUPLICATES _BOOST_USED_COMPONENTS)
set(BOOST_USED_COMPONENTS ${_BOOST_USED_COMPONENTS} CACHE STRING "Components" FORCE)

if(OPENMIND_BUILD_GC)
	add_definitions(-DOPENMIND_BUILD_GC)
endif()
if(OPENMIND_PRIME_MINING)
	add_definitions(-DOPENMIND_PRIME_MINING)
endif()

set(OPENMIND_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message("OPENMIND_INCLUDE_DIR ${OPENMIND_INCLUDE_DIR}")

find_package(Boost ${OPENMIND_PREFERRED_BOOST_VERSION}
	CONFIG
	HINTS C:/Boost
	COMPONENTS ${BOOST_USED_COMPONENTS}
	)
if(NOT Boost_FOUND)
	find_package(Boost ${OPENMIND_REQUIRED_BOOST_VERSION}
		CONFIG
		HINTS C:/Boost
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
	message("Boost_VERSION_STRING: ${Boost_VERSION_STRING}")
	message("Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
	message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
	message("BOOST_ADDITIONAL_COMPONENTS: ${BOOST_ADDITIONAL_COMPONENTS}")
	message("BOOST_USED_COMPONENTS: ${BOOST_USED_COMPONENTS}")
	message("Boost_LIBRARIES: ${Boost_LIBRARIES}")
	message("Boost_USE_STATIC_LIBS: ${Boost_USE_STATIC_LIBS}")

    foreach(_module ${BOOST_USED_COMPONENTS})
		string(TOUPPER ${_module} _module_upcase)
		message("Boost_${_module_upcase}_FOUND: ${Boost_${_module_upcase}_FOUND}")
		message("Boost_${_module_upcase}_LIBRARY: ${Boost_${_module_upcase}_LIBRARY}")
	endforeach()
endif()

option(OPENMIND_BUILD_BOOST "Fetch&build boost" ${OM_BUILD_BOOST})

if(OPENMIND_USE_CPACK)
	include(CPack)
	set(CPACK_BUNDLE_NAME ${PRODUCT_NAME} CACHE STRING "Binaries bundle name")
	if(APPLE)
		option(CPACK_BINARY_BUNDLE "Build macOS bundle" ON)
	elseif(WIN32)
		find_package(WIX)
		option(CPACK_BINARY_WIX "Build installer with WIX toolset" ${WIX_FOUND})
		find_package(NSIS)
		option(CPACK_BINARY_NSIS "Build installer with NSIS toolset" ${NSIS_FOUND})
	endif()
	option(CPACK_SOURCE_RPM "Sources" OFF)
	option(CPACK_SOURCE_TBZ2 "Sources" OFF)
	option(CPACK_SOURCE_TGZ "Sources" OFF)
	option(CPACK_SOURCE_TXZ "Sources" OFF)
	option(CPACK_SOURCE_TZ "Sources" OFF)
	option(CPACK_SOURCE_ZIP "Sources" OFF)
endif()

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

	add_custom_target(git-gui
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} gc
		COMMAND ${GIT_EXECUTABLE} gui
	)
	set_target_properties(git-gui PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util")

	add_custom_target(rebase-main
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash https://github.com/ohhmm/openmind main
	)
	set_target_properties(rebase-main PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util")

	add_custom_target(update
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash
		COMMAND ${GIT_EXECUTABLE} fetch --all
	)
	set_target_properties(update PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util")

	if(openmind_SOURCE_DIR AND NOT openmind_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
		add_custom_target(update-openmind
			WORKING_DIRECTORY ${openmind_SOURCE_DIR}
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash origin main
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash https://github.com/ohhmm/openmind main
			COMMAND ${GIT_EXECUTABLE} fetch --all
		)
		set_target_properties(update-openmind PROPERTIES
			EXCLUDE_FROM_ALL 1
			EXCLUDE_FROM_DEFAULT_BUILD 1
			FOLDER "util")
		add_dependencies(update update-openmind)

	endif()
endif()

find_package(OpenMP)
#if (OPENMP_FOUND)
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#elseif(NOT MSVC)
#   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
#endif()

if (Boost_FOUND)
	if(NOT TARGET boost)
		add_custom_target(boost)
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

    set(BOOST_BUILD_COMMAND "b2 address-model=${address_model} runtime-link=static,shared link=static,shared --disable-icu ${BOOST_BUILD_PARAMS}")
    if(WIN32)
    	if (CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "MinGW")
        	separate_arguments(BOOST_EXT_CONFIGURE WINDOWS_COMMAND "bootstrap.bat gcc")
	        separate_arguments(BOOST_BUILD_COMMAND WINDOWS_COMMAND "${BOOST_BUILD_COMMAND} toolset=gcc install")
    	else()
	        set(ENV{VS150COMNTOOLS} "C:/Program Files/Microsoft Visual Studio/Preview/Community/Common7/Tools")
	        separate_arguments(BOOST_EXT_CONFIGURE WINDOWS_COMMAND "bootstrap.bat msvc")
	        separate_arguments(BOOST_BUILD_COMMAND WINDOWS_COMMAND "${BOOST_BUILD_COMMAND} install")
		endif()
    else()
        set(BOOST_EXT_CONFIGURE ./bootstrap.sh)
		if(APPLE)
			set(BOOST_BUILD_APPLE_ARGS "-address-model=64 architecture=arm+x86 -s NO_LZMA=1 -s NO_ZSTD=1")
		endif()
        separate_arguments(BOOST_BUILD_COMMAND UNIX_COMMAND "./${BOOST_BUILD_COMMAND}")
        separate_arguments(BOOST_INSTALL_COMMAND UNIX_COMMAND "./${BOOST_BUILD_COMMAND} install ${BOOST_BUILD_APPLE_ARGS} --prefix='${CMAKE_BINARY_DIR}'")
    endif()

    message("ExternalProject_Add: ${BOOST_EXT_CONFIGURE} && ${BOOST_BUILD_COMMAND}")
    ExternalProject_Add(boost GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-${OPENMIND_PREFERRED_BOOST_VERSION}
        GIT_SHALLOW ON
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ${BOOST_EXT_CONFIGURE} --without-icu
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
add_dependencies(prerequisites boost)
if(TARGET boost)
	set_target_properties(boost PROPERTIES FOLDER "util")
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


# Find Python
if(NOT Python_FOUND)
    find_package(Python)
endif()
if(Python_FOUND)
    message(STATUS "Python found: ${Python_EXECUTABLE}")
    message(STATUS "Python version: ${Python_VERSION}")
endif()


if (OPENMIND_BUILD_TESTS)
    enable_testing()
	if(NOT MSVC)
		if(NOT APPLE)
    		set(TEST_DEPS "pthread")
		endif()
		if(NOT WIN32 AND OPENMIND_USE_TBB)
			set(TEST_DEPS ${TEST_DEPS} TBB::tbb)
		endif()
	endif()
	add_custom_target(disable_openmind_tests
		COMMAND ${CMAKE_COMMAND} -DOPENMIND_BUILD_TESTS=OFF "${CMAKE_CURRENT_SOURCE_DIR}")
	set_target_properties(disable_openmind_tests PROPERTIES FOLDER "util")
else()
    add_custom_target(enable_openmind_tests
        COMMAND ${CMAKE_COMMAND} -DOPENMIND_BUILD_TESTS=ON "${CMAKE_CURRENT_SOURCE_DIR}")
	set_target_properties(enable_openmind_tests PROPERTIES FOLDER "util")
endif()

macro(echo)
	message("${ARGN}: ${${ARGN}}")
endmacro(echo)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib
                    ${CMAKE_BINARY_DIR}/lib64
                    )

fold(
    omnn
    OpenMind
)

cmake_language(DEFER DIRECTORY ${CMAKE_SOURCE_DIR} CALL _openmind_hook_end_of_configure())
function(_openmind_hook_end_of_configure)
	if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.21)
		build_command(PREREQUISITES_BUILD_CMD TARGET prerequisites PARALLEL_LEVEL ${CMAKE_AUTOGEN_PARALLEL})
	else()
		build_command(PREREQUISITES_BUILD_CMD TARGET prerequisites)
	endif()
	message("Building target prerequisites in ${CMAKE_BINARY_DIR} for prerequisites:")
	execute_process(COMMAND ${PREREQUISITES_BUILD_CMD}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMAND_ECHO STDOUT 
	)
endfunction()
