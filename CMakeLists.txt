cmake_minimum_required(VERSION 3.14)

get_filename_component(PRODUCT_NAME ${CMAKE_SOURCE_DIR} NAME_WE)
string(REGEX REPLACE "/" "\\\\" PRODUCT_NAME ${PRODUCT_NAME})
project(${PRODUCT_NAME} CXX)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
if(NOT CMAKE_MSVC_RUNTIME_LIBRARY)
	set_property(GLOBAL PROPERTY CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:DebugDLL>")
endif()

message("BIN_DIR=${CMAKE_BINARY_DIR}")
message("SRC_DIR=${CMAKE_SOURCE_DIR}")
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	message("OPENMIND_DIR=${CMAKE_CURRENT_SOURCE_DIR}")
	if(EXISTS "${CMAKE_SOURCE_DIR}/../openmind/CMakeLists.txt")
		get_filename_component(SIBLING_PROJECTS_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
		message("SIBLING_PROJECTS_DIR=${SIBLING_PROJECTS_DIR}")
		if(NOT FETCHCONTENT_SOURCE_DIR_OPENMIND STREQUAL "${SIBLING_PROJECTS_DIR}/openmind")
			message("Updating FETCHCONTENT_SOURCE_DIR_OPENMIND=${FETCHCONTENT_SOURCE_DIR_OPENMIND}")
			set(FETCHCONTENT_SOURCE_DIR_OPENMIND "${SIBLING_PROJECTS_DIR}/openmind" CACHE PATH "OpenMind directory path" FORCE)
		endif()
	endif()
	message("FETCHCONTENT_SOURCE_DIR_OPENMIND=${FETCHCONTENT_SOURCE_DIR_OPENMIND}")
endif()
message("PRODUCT_NAME=${PRODUCT_NAME}")
message("CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
message("CMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}")
if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
	EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
	set(CMAKE_OSX_ARCHITECTURES ${ARCHITECTURE} CACHE PATH "May be used for cross-compilation" FORCE)
	message("CMAKE_OSX_ARCHITECTURES=${ARCHITECTURE}")
endif()

if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(WARNING "CMake build should be run in a separate 'build' directory. Please create a 'build' directory and run 'cmake ..' from there.")
endif()

set(OPENMIND_PRIME_TABLE Boost CACHE STRING "Table of predefined prime numbers")

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
  message(STATUS "Multi-configuration generator detected.")
else()
  message(STATUS "Single-configuration generator detected.")
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type for single-configuration generators" FORCE)
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/"
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/"
	"${CMAKE_BINARY_DIR}/lib/cmake"
	"${CMAKE_SOURCE_DIR}/cmake"
)
if(OPENMIND_USE_CONAN)
	list(PREPEND CMAKE_MODULE_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"${CMAKE_CURRENT_BINARY_DIR}"
	)
	list(APPEND CMAKE_PREFIX_PATH ${CMAKE_MODULE_PATH})
endif()

include(bins)
include(sanitizer)

message("address_model: ${address_model}")


if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
	set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} CACHE PATH "hint for find_package")
	set(_IS_DEBUG ON)
	if(MSVC)
		add_compile_options($<$<CONFIG:Debug>:/constexpr:steps1000000000>)
		add_link_options(/stack:1000000000)
	elseif(NOT OPENMIND_USE_VCPKG)
		add_definitions(-DBOOST_ALL_DYN_LINK)
	endif()
else()
	set(_IS_DEBUG NO)
	set(Boost_USE_STATIC_LIBS ON)
	SET(BUILD_SHARED_LIBS NO)
	if(NOT OPENMIND_USE_VCPKG)
		add_definitions(-DBOOST_ALL_STATIC_LINK)
	endif()
	if(CMAKE_BUILD_TYPE MATCHES [Rr][Ee][Ll][Ee][Aa][Ss][Ee])
		set(OPENMIND_PRIME_TABLE Bundled CACHE STRING "Table of predefined prime numbers")
		option(OPENMIND_USE_CPACK "Build an installer" ON)
	elseif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
		message(WARNING "Consider using multi-config generator or setting CMAKE_BUILD_TYPE")
	endif()
endif()
set(OPENMIND_PRIME_TABLE_OPTIONS Boost;Bundled)

if(BUILD_TESTS OR OPENMIND_BUILD_TESTS OR _IS_DEBUG OR NOT CMAKE_BUILD_TYPE)
	set(initTestsEnabled ON)
else()
	set(initTestsEnabled NO)
endif()
set(initUtilsEnabled ${initTestsEnabled})
set(initBenchmarksEnabled ${initTestsEnabled})
option(OPENMIND_BUILD_3RD_PARTY_TESTS "Build various 3rd party TESTS to compare to openmind by performance" NO)
option(OPENMIND_BUILD_BENCHMARKS "Build various benchmarks" ${initBenchmarksEnabled})
option(OPENMIND_BUILD_GC "Use garbage collection. Faster but requires abundant memory." NO)
option(OPENMIND_BUILD_SAMPLES "Build various samples" NO)
option(OPENMIND_BUILD_TESTS "Build various tests" ${initTestsEnabled})
option(OPENMIND_BUILD_UTIILS "Build various utils" ${initUtilsEnabled})
option(OPENMIND_DEBUG_CHECKS "Additional debug checks (NOOMDEBUG define)" ${_IS_DEBUG})
option(OPENMIND_DEPEND_ON_PREREQUISITES "Better consider installing prerequisites by building the target 'prerequisites' upon initial setup. This way its not checked each build." NO)
option(OPENMIND_PRIME_MINING "Extend primes table by appending to Primes.inc source code file during CPU cores idle" ${_IS_DEBUG})
option(OPENMIND_UPDATE_EXTERNAL_DEPS "Update ExternalProject dependencies" NO)
option(OPENMIND_USE_QT "Link Qt libs" NO)

include(intel)

option(OPENMIND_USE_OPENCL "Use OpenCL calculations" ${OpenCL_FOUND})
if(NOT OPENMIND_USE_OPENCL)
    message(WARNING "Building without OpenCL support. Some computations may be slower.")
endif()
if(OPENMIND_USE_OPENCL)
	find_optional_pkg(OpenGL)
	option(OPENMIND_USE_OPENGL "Use OpenGL viewport" ${OpenGL_FOUND})
endif()

if(NOT WIN32)
	find_package(TBB)
	option(OPENMIND_USE_TBB "Link TBB" ${TBB_FOUND})
	if(OPENMIND_USE_TBB)
		add_definitions(-DOPENMIND_USE_TBB)
	endif()
endif()
set(OPENMIND_PREFERRED_BOOST_VERSION 1.87.0 CACHE STRING "Boost library version to use")
set(OPENMIND_REQUIRED_BOOST_VERSION 1.81.0 CACHE STRING "Boost library minimum compatible version to use")
set_property(CACHE OPENMIND_PRIME_TABLE PROPERTY STRINGS ${OPENMIND_PRIME_TABLE_OPTIONS})

if(MSVC AND ${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24.0")
    message("Using Edit&Continue policy")
    cmake_policy(SET CMP0138 NEW)
endif()
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.19.0")
    cmake_policy(SET CMP0114 NEW)
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(OPENMIND_PRIME_TABLE STREQUAL Boost)
	add_definitions(-DOPENMIND_PRIME_TABLE_BOOST)
elseif(OPENMIND_PRIME_TABLE STREQUAL Bundled)
	add_definitions(-DOPENMIND_PRIME_TABLE_OM)
endif()
set(_BOOST_USED_COMPONENTS
	${BOOST_USED_COMPONENTS}
	${BOOST_ADDITIONAL_COMPONENTS}
	chrono
	date_time
	filesystem
	iostreams
	locale
	python
	serialization
	system
	thread
	CACHE STRING "Components" FORCE)
if(OPENMIND_USE_VCPKG)
#	set(Boost_USE_STATIC_LIBS        ON)
#	set(Boost_USE_DEBUG_LIBS        NO)
#	set(Boost_USE_RELEASE_LIBS       ON)
#	set(Boost_USE_MULTITHREADED      ON)
#	set(Boost_USE_STATIC_RUNTIME     ON)
	set(_BOOST_USED_COMPONENTS ${_BOOST_USED_COMPONENTS}
		compute
		multiprecision
		uuid
		CACHE STRING "Components" FORCE)
endif(OPENMIND_USE_VCPKG)
if(BUILD_TESTS OR OPENMIND_BUILD_TESTS OR _IS_DEBUG OR NOT CMAKE_BUILD_TYPE)
    enable_testing()
	set(_BOOST_USED_COMPONENTS ${_BOOST_USED_COMPONENTS}
		unit_test_framework
		CACHE STRING "Components" FORCE)
	if(NOT OPENMIND_USE_VCPKG)
		set(_BOOST_USED_COMPONENTS ${_BOOST_USED_COMPONENTS}
			test_exec_monitor
			CACHE STRING "Components" FORCE)
	endif()
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
message("OPENMIND_INCLUDE_DIR=${OPENMIND_INCLUDE_DIR}")

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	option(OPENMIND_USE_CONAN "Use conan dependencies" ON)
	if(OPENMIND_USE_CONAN)
		message("Using conanbuildinfo.cmake")
		include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
		conan_basic_setup(TARGETS)
	endif()
elseif(OPENMIND_USE_CONAN)
	if(EXISTS ${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake)
		message("conanbuildinfo.cmake created in source directory")
		include(${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake)
		conan_basic_setup(TARGETS)
	else()
		message("conanbuildinfo.cmake not found. Try building Install__Conan target.")
	endif()
endif()

if (NOT Boost_FOUND)
	if (OPENMIND_USE_VCPKG)
		unset(Boost_DIR CACHE)
		unset(Boost_INCLUDE_DIR CACHE)
		find_package(Boost CONFIG COMPONENTS ${BOOST_USED_COMPONENTS} REQUIRED)
	else()
		set(BOOST_PATHS_HINTS
				C:/Boost
				/usr/local/lib/cmake/Boost-*
				/usr/lib/x86_64-linux-gnu/cmake/Boost-*
				/usr/local/share/boost-*
		)
		find_package(Boost ${OPENMIND_PREFERRED_BOOST_VERSION}
				CONFIG
				HINTS ${BOOST_PATHS_HINTS}
				COMPONENTS ${BOOST_USED_COMPONENTS}
		)
		if(NOT Boost_FOUND)
			find_package(Boost ${OPENMIND_REQUIRED_BOOST_VERSION}
					CONFIG
					HINTS ${BOOST_PATHS_HINTS}
					COMPONENTS ${BOOST_USED_COMPONENTS}
			)
		endif()
	endif ()
endif ()
add_custom_target(prerequisites)
set_target_properties(prerequisites PROPERTIES FOLDER "util")


if(NOT Boost_FOUND
	AND NOT OPENMIND_USE_CONAN
	AND NOT OPENMIND_USE_VCPKG
	)
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

include(packaging)
include(ExternalData)
include(ExternalProject)
find_package(OpenMP)
#if (OPENMP_FOUND)
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#elseif(NOT MSVC)
#   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
#endif()

if (Boost_FOUND OR OPENMIND_USE_CONAN OR OPENMIND_USE_VCPKG)
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
    	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
			separate_arguments(BOOST_EXT_CONFIGURE WINDOWS_COMMAND "bootstrap.bat clang")
			separate_arguments(BOOST_BUILD_COMMAND WINDOWS_COMMAND "${BOOST_BUILD_COMMAND} toolset=clang install")
    	elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "MinGW")
        	separate_arguments(BOOST_EXT_CONFIGURE WINDOWS_COMMAND "bootstrap.bat gcc")
	        separate_arguments(BOOST_BUILD_COMMAND WINDOWS_COMMAND "${BOOST_BUILD_COMMAND} toolset=gcc install")
		else()
	        set(ENV{VS150COMNTOOLS} "C:/Program Files/Microsoft Visual Studio/Preview/Community/Common7/Tools")
	        separate_arguments(BOOST_EXT_CONFIGURE WINDOWS_COMMAND "bootstrap.bat msvc")
	        separate_arguments(BOOST_BUILD_COMMAND WINDOWS_COMMAND "${BOOST_BUILD_COMMAND} install")
		endif()
    else()
		if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
			set(BOOST_TOOLSET clang)
		endif()
		if(APPLE)
			set(BOOST_BUILD_APPLE_ARGS "-address-model=64 architecture=arm+x86 -s NO_LZMA=1 -s NO_ZSTD=1")
		endif()
		separate_arguments(BOOST_EXT_CONFIGURE UNIX_COMMAND "./bootstrap.sh ${BOOST_TOOLSET}")
        separate_arguments(BOOST_BUILD_COMMAND UNIX_COMMAND "./${BOOST_BUILD_COMMAND} --cxx=${CMAKE_CXX_COMPILER}")
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
    Boost::chrono
    Boost::filesystem
    Boost::serialization
    Boost::system
    )
if(NOT MSVC
	OR Boost_VERSION VERSION_LESS 1.80.0
	OR OPENMIND_USE_CONAN
)
    set(BOOST_LINK_LIBS ${BOOST_LINK_LIBS}
        Boost::thread)
endif()

set(BOOST_TEST_LINK_LIBS
	${BOOST_LINK_LIBS}
	Boost::unit_test_framework
)
if(OPENMIND_USE_VCPKG)
    list(APPEND BOOST_LINK_LIBS
        Boost::headers
    )
endif()
if(NOT MSVC AND NOT OPENMIND_USE_VCPKG AND NOT OPENMIND_USE_CONAN)
	list(APPEND BOOST_TEST_LINK_LIBS Boost::test_exec_monitor)
endif()
set(BOOST_LINK_LIBS ${BOOST_LINK_LIBS} CACHE STRING "Boost libraries" FORCE)
set(BOOST_TEST_LINK_LIBS ${BOOST_TEST_LINK_LIBS} CACHE STRING "Boost test libraries" FORCE)

IF (Boost_FOUND)
    message(STATUS "Boost found:")
    message(STATUS "  Boost_DIR: ${Boost_DIR}")
    message(STATUS "  Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
    message(STATUS "  EXTERNAL_FETCHED_BOOST: ${EXTERNAL_FETCHED_BOOST}")
    message(STATUS "  Boost_LIBRARY_DIR: ${Boost_LIBRARY_DIR}")
    message(STATUS "  Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
    message(STATUS "  Boost_VERSION: ${Boost_VERSION}")
    message(STATUS "  Boost_LIBRARIES: ${Boost_LIBRARIES}")
ELSE()
    message(WARNING "Boost not found. Build 'prerequisites' target first.")
ENDIF()

if (OPENMIND_BUILD_TESTS)
	if(NOT MSVC)
		if(NOT APPLE)
			set(TEST_DEPS ${TEST_DEPS} pthread)
		endif()
		if(NOT WIN32 AND OPENMIND_USE_TBB)
			set(TEST_DEPS ${TEST_DEPS} TBB::tbb)
		endif()
	endif()
	add_custom_target(disable_openmind_tests
		COMMAND ${CMAKE_COMMAND} -DOPENMIND_BUILD_TESTS=NO "${CMAKE_CURRENT_SOURCE_DIR}")
	set_target_properties(disable_openmind_tests PROPERTIES FOLDER "util")
else()
    add_custom_target(enable_openmind_tests
        COMMAND ${CMAKE_COMMAND} -DOPENMIND_BUILD_TESTS=ON "${CMAKE_CURRENT_SOURCE_DIR}")
	set_target_properties(enable_openmind_tests PROPERTIES FOLDER "test")
endif()

macro(echo)
	message("${ARGN}: ${${ARGN}}")
endmacro(echo)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib
                    ${CMAKE_BINARY_DIR}/lib64
                    )

fold(
	cmake
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
