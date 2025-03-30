include(deps)
include(fold)
include(pch)
include(qtect)

if(WIN32)
	set(platform windows CACHE STRING "")
	if(EXISTS $ENV{VCToolsInstallDir})
		string(REGEX REPLACE "\\\\" "/" VCToolsInstallDir $ENV{VCToolsInstallDir})
	endif()
elseif(APPLE)
	set(platform apple CACHE STRING "")
else()
	set(platform posix CACHE STRING "")
endif()

macro(get_target_name)
    get_filename_component(${ARGN} ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	#block()
		set(dir ${CMAKE_CURRENT_SOURCE_DIR})
		while(${ARGN} STREQUAL platform)
			get_filename_component(dir ${dir} DIRECTORY)
			get_filename_component(${ARGN} ${dir} NAME)
		endwhile()
	#endblock()
	unset(${ARGN}_n)
	while(TARGET ${${ARGN}}${${ARGN}_n})
		math(EXPR ${ARGN}_n "${${ARGN}_n} + 1")
	endwhile()
	set(${ARGN} ${${ARGN}}${${ARGN}_n})
endmacro(get_target_name)

macro(glob_add_dir_source_files)
    if(${ARGN} STREQUAL ".")
        set(varnameprefix)
    else()
        set(varnameprefix ${ARGN}_)
    endif()

	if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}
    	AND (NOT varnameprefix OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}/CMakeLists.txt)
        )

		file(GLOB ${varnameprefix}headers ${ARGN}/*.h ${ARGN}/*.hpp ${ARGN}/*.inc ${ARGN}/*.hxx)
		file(GLOB ${varnameprefix}interfaces ${ARGN}/*.ixx)
		file(GLOB ${varnameprefix}src ${ARGN}/*.ixx ${ARGN}/*.cpp ${ARGN}/*.cppm ${ARGN}/*.c ${ARGN}/*.CC ${ARGN}/*.cc ${ARGN}/*.m ${ARGN}/*.mm)
		file(GLOB ${varnameprefix}qmlsrc ${ARGN}/*.qml)
		file(GLOB ${varnameprefix}qtsrc ${ARGN}/*.qrc)
		file(GLOB ${varnameprefix}resources ${ARGN}/*.rc)
		file(GLOB ${varnameprefix}pch ${ARGN}/pch.h ${ARGN}/stdafx.h)
		file(GLOB ${varnameprefix}pcs ${ARGN}/pch.cpp ${ARGN}/stdafx.cpp)
		file(GLOB ${varnameprefix}script
			${ARGN}/*.js ${ARGN}/*.ts
			${ARGN}/*.php
			${ARGN}/*.pl
			${ARGN}/*.py
			${ARGN}/*.rb
			${ARGN}/*.skrypt
			${ARGN}/*.vb
			)
		list(APPEND ${varnameprefix}all_source_files
			${${varnameprefix}headers}
			${${varnameprefix}interfaces}
			${${varnameprefix}src}
			${${varnameprefix}qmlsrc}
			${${varnameprefix}qtsrc}
			${${varnameprefix}resources}
			${${varnameprefix}pch}
			${${varnameprefix}pcs}
			${${varnameprefix}script}
			)
        if(varnameprefix)
		    list(APPEND headers ${${varnameprefix}headers})
		    list(APPEND interfaces ${${varnameprefix}interfaces})
		    list(APPEND src ${${varnameprefix}src})
		    list(APPEND qmlsrc ${${varnameprefix}qmlsrc})
		    list(APPEND qtsrc ${${varnameprefix}qtsrc})
		    list(APPEND resources ${${varnameprefix}resources})
		    list(APPEND pch ${${varnameprefix}pch})
		    list(APPEND pcs ${${varnameprefix}pcs})
			list(APPEND script ${${varnameprefix}script})
			list(APPEND all_source_files ${${varnameprefix}all_source_files})
        endif(varnameprefix)
    else()
		unset(${varnameprefix}all_source_files)
		unset(${varnameprefix}headers)
		unset(${varnameprefix}interfaces)
		unset(${varnameprefix}pch)
		unset(${varnameprefix}pcs)
		unset(${varnameprefix}qmlsrc)
		unset(${varnameprefix}qtsrc)
		unset(${varnameprefix}resources)
		unset(${varnameprefix}script)
    	unset(${varnameprefix}src)
	endif()
endmacro(glob_add_dir_source_files)


macro(glob_source_files)
    glob_add_dir_source_files(.)
    glob_add_dir_source_files(${platform})
    glob_add_dir_source_files(src)
    glob_add_dir_source_files(cpp)
    SET_SOURCE_FILES_PROPERTIES(${headers} PROPERTIES HEADER_FILE_ONLY ON)
endmacro(glob_source_files)

macro(apply_target_commons this_target)
	string(REPLACE "-" "" dashless ${this_target})
	string(TOUPPER ${dashless} this_target_up)
	set(defs 
		HAS_BOOST
		BOOST_SYSTEM_NO_DEPRECATED
		BOOST_ERROR_CODE_HEADER_ONLY
		BOOST_COMPUTE_USE_CPP11
		_CRT_SECURE_NO_WARNINGS
		-DCMAKE_COMMAND="${CMAKE_COMMAND}"
		-DGIT_EXECUTABLE_PATH="${GIT_EXECUTABLE}"
		-DBUILD_DIR="${CMAKE_BINARY_DIR}/"
		-DSRC_DIR="${CMAKE_SOURCE_DIR}/"
		-D_${dashless}_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
		-D_${this_target_up}_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
	)
	set(opts
		$<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<CXX_COMPILER_ID:Clang>>>:-fcoroutines>
		$<$<CXX_COMPILER_ID:GNU>:-fmodules-ts>
	)
	if(OPENMIND_USE_OPENCL)
		set(defs ${defs}
			OPENMIND_USE_OPENCL
			OPENMIND_MATH_USE_OPENCL
			)
		if(OPENMIND_DEBUG_CHECKS)
			set(defs ${defs} BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION)
		endif()
		if(OPENMIND_USE_OPENGL)
			set(defs ${defs} OPENMIND_USE_OPENGL)
		endif()
	endif()
	if(NOT OPENMIND_DEBUG_CHECKS)
		set(defs ${defs} NOOMDEBUG)
	endif()
	set_target_properties(${this_target} PROPERTIES
		CXX_STANDARD 23
		CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON
		EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
		MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:DebugDLL>
		POSITION_INDEPENDENT_CODE ON
		)
	if(OPENMIND_USE_OPENCL_INTEL_SYCL)
		set(opts ${opts} ${SYCL_FLAGS})
		set(incls ${incls} 
			"${IntelSYCL_DIR}/../../../include"
			"${IntelSYCL_DIR}/../../../include/sycl"
		)
	endif(OPENMIND_USE_OPENCL_INTEL_SYCL)
    set(ldirs
		${Boost_LIBRARY_DIRS}
		${CMAKE_BINARY_DIR}/bin
		)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(defs ${defs} 
			APPLE_CONSTEXPR=
			NO_APPLE_CONSTEXPR=constexpr
			NO_CLANG_CONSTEXPR=constexpr
			MSVC_CONSTEXPR=constexpr
			NO_MSVC_CONSTEXPR=
			MSVC
			)
		if(NOT OPENMIND_USE_VCPKG)
			set(defs ${defs} 
				$<$<CONFIG:DEBUG>:BOOST_ALL_STATIC_LINK>
				$<$<CONFIG:Release>:BOOST_ALL_STATIC_LINK>
				$<$<CONFIG:RelWithDebInfo>:BOOST_ALL_STATIC_LINK>
				$<$<CONFIG:MinSizeRel>:BOOST_ALL_STATIC_LINK>
				)
		endif()
		if(OPENMIND_USE_CONAN OR OPENMIND_USE_VCPKG)
			set(defs ${defs} 
				BOOST_ALL_NO_LIB
				)
		endif()
		set(opts ${opts}
			$<$<CONFIG:DEBUG>:/ZI>
			$<$<CONFIG:Release>:/MT>
			$<$<CONFIG:RelWithDebInfo>:/ZI>
			/bigobj
			/constexpr:steps1000000000
			/experimental:module
			/FS
			/MP
			/source-charset:utf-8
			/std:c++latest
			)
		#target_compile_options(${this_target} INTERFACE REMOVE
		#	$<$<CONFIG:DEBUG>:/Zi>
		#	$<$<CONFIG:RelWithDebInfo>:/Zi>
		#)
		set(lopts
			$<$<CONFIG:DEBUG>:/LTCG:OFF>
			$<$<CONFIG:RelWithDebInfo>:/LTCG:OFF>
			$<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<CXX_COMPILER_ID:Clang>>>:-wunicode>
			)
		message("${CMAKE_BINARY_DIR}/bin")
        if(EXISTS ${Boost_INCLUDE_DIR}/stage/lib)
			set(ldirs ${Boost_INCLUDE_DIR}/stage/lib)
        endif()
        if(EXISTS ${EXTERNAL_FETCHED_BOOST}/stage/lib)
            set(ldirs ${EXTERNAL_FETCHED_BOOST}/stage/lib)
        endif()
        if(EXISTS C:/Boost/lib)
            set(ldirs C:/Boost/lib)
        endif()
	else()
		set(defs ${defs} 
			MSVC_CONSTEXPR=
            NO_MSVC_CONSTEXPR=constexpr
			)
		if(APPLE)
			set(defs ${defs} 
				APPLE_CONSTEXPR=constexpr
				NO_APPLE_CONSTEXPR=
				NO_CLANG_CONSTEXPR=
				)
		else()
			set(defs ${defs} 
				APPLE_CONSTEXPR=
				NO_APPLE_CONSTEXPR=constexpr
                $<$<CXX_COMPILER_ID:Clang>:NO_CLANG_CONSTEXPR=>
                $<$<NOT:$<CXX_COMPILER_ID:Clang>>:NO_CLANG_CONSTEXPR=constexpr>
				)
		endif()
	endif()

	set(incls ${incls}  ${CMAKE_CURRENT_SOURCE_DIR})
	if(Boost_INCLUDE_DIR)
		set(incls ${incls}  ${Boost_INCLUDE_DIR})
	endif()
	if(Boost_INCLUDE_DIRS)
		set(incls ${incls}  ${Boost_INCLUDE_DIRS})
	endif()
	if(EXTERNAL_FETCHED_BOOST)
		set(incls ${incls}  ${EXTERNAL_FETCHED_BOOST})
	endif()

	target_include_directories(${this_target} INTERFACE ${incls})
    target_link_directories(${this_target} INTERFACE ${ldirs})
	get_target_property(target_type ${this_target} TYPE)
	if(target_type STREQUAL "INTERFACE_LIBRARY")
		set(visibility INTERFACE PARENT_SCOPE)
		message(STATUS "${this_target} is INTERFACE")
		set(defs INTERFACE ${defs})
		set(opts INTERFACE ${opts})
		target_compile_features(${this_target} INTERFACE cxx_std_23)
		set(lopts INTERFACE ${lopts})
		target_include_directories(${this_target} INTERFACE
		    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			$<INSTALL_INTERFACE:include>
        )
	else()
		set(visibility PUBLIC PARENT_SCOPE)
		set(defs INTERFACE ${defs} PUBLIC ${defs})
		set(opts INTERFACE ${opts} PUBLIC ${opts})
		target_compile_features(${this_target} INTERFACE cxx_std_23 PUBLIC cxx_std_23)
		target_include_directories(${this_target} PUBLIC ${incls})
		set(lopts INTERFACE ${lopts} PUBLIC ${lopts})
	    target_link_directories(${this_target} PUBLIC ${ldirs})
	endif()
	target_compile_definitions(${this_target} ${defs})
	target_compile_options(${this_target} ${opts})
	target_link_options(${this_target} ${lopts})

	if (target_type STREQUAL "EXECUTABLE")

	endif ()
endmacro(apply_target_commons)

macro(link_boost_libs)
	if(Boost_FOUND)
		if(NOT MSVC OR OPENMIND_USE_CONAN OR OPENMIND_USE_VCPKG)
			foreach (boostlibtarget ${BOOST_LINK_LIBS})
				if(TARGET ${boostlibtarget})
					message("linking boostlibtarget: ${boostlibtarget}")
					target_link_libraries(${this_target} ${visibility} ${boostlibtarget})
				else()
					message("skipping linking ${boostlibtarget}, the target not found")
				endif()
			endforeach()
			foreach (boostcomponent ${BOOST_ADDITIONAL_COMPONENTS})
				if(TARGET Boost::${boostcomponent})
					message("linking boostlibtarget: Boost::${boostcomponent}")
					target_link_libraries(${this_target} ${visibility} Boost::${boostcomponent})
				else()
					string(TOUPPER ${boostcomponent} boostcomponent)
					message("include ${boostcomponent}: ${Boost_${boostcomponent}_INCLUDE_DIR}")
					target_link_libraries(${this_target} ${visibility} ${Boost_${boostcomponent}_INCLUDE_DIR})
					message("linking ${boostcomponent}: ${Boost_${boostcomponent}_LIBRARY}")
					target_link_libraries(${this_target} ${visibility} ${Boost_${boostcomponent}_LIBRARY})
				endif()
			endforeach()
		endif()
	endif()
endmacro()

function(apply_test_commons this_target)
	get_target_property(target_type ${this_target} TYPE)
	if(target_type STREQUAL "INTERFACE_LIBRARY")
		set(visibility INTERFACE)
	else()
		set(visibility PUBLIC)
	endif()
	foreach(dir
		/usr/local/lib
		${Boost_INCLUDE_DIR}/stage/lib
		${Boost_INCLUDE_DIR}/../../lib
		${EXTERNAL_FETCHED_BOOST}/stage/lib
		${EXTERNAL_FETCHED_BOOST}/../../lib
		${CMAKE_BINARY_DIR}/lib
		${CMAKE_BINARY_DIR}/lib64
		)
		if(EXISTS ${dir})
			target_link_directories(${this_target} ${visibility} ${dir})
		endif()
	endforeach()

    set_target_properties(${this_target} PROPERTIES
            FOLDER "test"
    )
    target_compile_definitions(${this_target} ${visibility}
            -DTEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
            -DTEST_BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}/"
    )
    target_link_options(${this_target} ${visibility}
            $<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<CXX_COMPILER_ID:Clang>>,$<PLATFORM_ID:Windows>>:-mconsole>
    )
    if(ENABLE_ASAN)
        message("Enabling AddressSanitizer for ${this_target}")
        target_enable_asan(${this_target})
    endif()

    if(Boost_FOUND)
		message("Boost_FOUND: ${Boost_FOUND}")
		if(NOT MSVC OR OPENMIND_USE_CONAN)
			message("NOT MSVC OR OPENMIND_USE_CONAN, using deps ${BOOST_TEST_LINK_LIBS}")
			deps(${BOOST_TEST_LINK_LIBS})
		endif()
	endif()
endfunction(apply_test_commons)

function(test)
    string(STRIP "${ARGN}" test_libs)
	get_filename_component(parent_target ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
	get_filename_component(parent_target ${parent_target} NAME)
	project(${parent_target})
	message("\nCreating Tests for ${parent_target}")
	glob_source_files()
	message("test_libs ${test_libs}")
	set(libs ${test_libs} ${TEST_DEPS})
	if(Boost_FOUND)
		if(NOT MSVC OR OPENMIND_USE_CONAN OR OPENMIND_USE_VCPKG)
			set(libs ${libs} ${BOOST_TEST_LINK_LIBS})
		endif()
	endif()
    if(OPENMIND_USE_QT)
		qtect()
    endif()
	message("libs ${libs}")
	foreach(tg ${libs})
		message("${tg}")
		if(TARGET ${tg})
			get_target_property(type ${tg} TYPE)
			message("${tg} target type is ${type}")
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
		file(GLOB testfiles ${TEST_NAME}.*)
	    message("test ${TEST_NAME}: ${testfiles}")
	    add_executable(${TEST_NAME} ${testfiles})

		set(this_target ${TEST_NAME})
		apply_target_commons(${TEST_NAME})
		apply_test_commons(${TEST_NAME})
		target_link_libraries(${TEST_NAME} PUBLIC
			${parent_target}
			testlibs
		)

		message("using deps ${libs}")
		deps(${libs})

        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.db)
        add_test(NAME ${TEST_NAME}
            COMMAND $<TARGET_FILE:${TEST_NAME}>
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.db
	    )
	    if(NOT "${dep_on_targets}" STREQUAL "")
	    	message("making ${TEST_NAME} depend on ${dep_on_targets}")
			add_dependencies(${TEST_NAME} ${parent_target} ${dep_on_targets})
		endif()
	endforeach()
endfunction()


macro(lib)
    string(STRIP "${ARGN}" deps)
    get_target_name(this_target)
    set(${this_target}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE FILEPATH "${this_target} include path")
	string(TOLOWER ${this_target} this_target_name_lowcase)
    project(${this_target})
    message("\nCreating Library: ${this_target}")
	check_dep_file()
	glob_source_files()
	if(NOT all_source_files)
		set(USE_SHARED INTERFACE)
	endif()
    add_library(${this_target} ${USE_SHARED} ${all_source_files})
	set(incls
        ${OPENMIND_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${${this_target}_INCLUDE_DIR}
        )
	APPLY_TARGET_COMMONS(${this_target})
	if(target_type STREQUAL "INTERFACE_LIBRARY")
		target_include_directories(${this_target} ${visibility}
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			$<INSTALL_INTERFACE:include>
		)
	endif()

	if(CMAKE_CXX_STANDARD)
		set_target_properties(${this_target} PROPERTIES CXX_STANDARD ${CMAKE_CXX_STANDARD})
	endif(CMAKE_CXX_STANDARD)
    set_target_properties(${this_target} PROPERTIES
		FOLDER "libs"
		PUBLIC_HEADER "${headers}"
		)
	message("add_library(${this_target} ${src})")
	link_boost_libs()
    deps(${deps})
	if(OPENMIND_USE_TBB)
		target_link_libraries(${this_target} ${visibility} tbb)
	endif()

    set(INSTALL_PARAMS TARGETS ${this_target})
	if(LIBRARY_DESTINATION)
		set(INSTALL_PARAMS ${INSTALL_PARAMS} LIBRARY DESTINATION ${LIBRARY_DESTINATION})
	endif()
	if(RUNTIME_DESTINATION)
		set(INSTALL_PARAMS ${INSTALL_PARAMS} RUNTIME DESTINATION ${RUNTIME_DESTINATION})
	endif()
	install(${INSTALL_PARAMS})

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


macro(dll)
	set(USE_SHARED SHARED)
	unset(CMAKE_SHARED_LIBRARY_PREFIX)
	lib(${ARGN})
	unset(USE_SHARED)
endmacro()


macro(mod)
	set(USE_SHARED MODULE)
	if(NOT LIBRARY_DESTINATION)
		set(LIBRARY_DESTINATION lib)
	endif()
	if(NOT RUNTIME_DESTINATION)
		set(RUNTIME_DESTINATION ${LIBRARY_DESTINATION})
	endif()
	lib(${ARGN})
	set_target_properties(${this_target} PROPERTIES
		PREFIX ""
		POSITION_INDEPENDENT_CODE ON
	)
	unset(USE_SHARED)
	unset(LIBRARY_DESTINATION)
	unset(RUNTIME_DESTINATION)
endmacro()


macro(exe)
    string(STRIP "${ARGN}" deps)

    get_target_name(this_target)

    project(${this_target})
    message("\nCreating Executable: ${this_target}")
	glob_source_files()
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
        list(APPEND all_source_files ${qml_QRC})
        qt_add_executable(${this_target} MANUAL_FINALIZATION ${all_source_files})
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
        add_library(${this_target} SHARED ${all_source_files})
	else()
		add_executable(${this_target} ${all_source_files})
    endif()
	if(MSVC)
		#TODO message("CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:Windows")
	endif()

    APPLY_TARGET_COMMONS(${this_target})
	SET_TARGET_PROPERTIES(${this_target} PROPERTIES LINKER_LANGUAGE CXX)

    #add_dependencies(${this_target} prerequisites)
	get_target_property(built ${this_target} EXCLUDE_FROM_ALL)
	if(built)
		if(Qt${QT_VERSION_MAJOR}_FOUND)
			if(APPLE AND PROJECT_VERSION)
				set_target_properties(${this_target} PROPERTIES
					MACOSX_BUNDLE_GUI_IDENTIFIER ${this_target}.deduction-fw.org
					MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
					MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
				)
			endif()
			set_target_properties(${this_target} PROPERTIES
				MACOSX_BUNDLE ON
				WIN32_EXECUTABLE ON
			)
			install(TARGETS ${this_target} BUNDLE DESTINATION .
				LIBRARY DESTINATION lib
				ARCHIVE DESTINATION lib
				PUBLIC_HEADER DESTINATION include)
		else()
			install(TARGETS ${this_target}
				LIBRARY DESTINATION lib
				ARCHIVE DESTINATION lib
				PUBLIC_HEADER DESTINATION include)
		endif()
	endif()

    target_include_directories(${this_target} PUBLIC
        ${OPENMIND_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}/include
        ${${this_target}_INCLUDE_DIR}
        )

    foreach(dir
        /usr/local/lib
        ${Boost_INCLUDE_DIR}/stage/lib
        ${Boost_INCLUDE_DIR}/../../lib
        ${EXTERNAL_FETCHED_BOOST}/stage/lib
        ${EXTERNAL_FETCHED_BOOST}/../../lib
        ${CMAKE_BINARY_DIR}/lib
        ${CMAKE_BINARY_DIR}/lib64
        )

        if(EXISTS ${dir})
            target_link_directories(${this_target} PUBLIC ${dir})
        endif()

    endforeach()

    foreach(item ${deps})
        message("adding ${item} sublib ${LIB} into ${this_target}")
        if(TARGET ${item})
            message("Linking target ${item}")
            target_link_libraries(${this_target} PUBLIC ${item})
            if(${item}_LIBRARIES)
                message("Linking ${item}_LIBRARIES ${${item}_LIBRARIES}")
                deps(${${item}_LIBRARIES})
                if(NOT "${${item}_LIBRARIES}" MATCHES ${item})
                    message(WARNING "${item}_LIBRARIES '${${item}_LIBRARIES}' has no ${item}")
                endif()
            endif()
        elseif(${item}_LIBRARIES)
            message("Linking ${item}_LIBRARIES ${${item}_LIBRARIES}")
			foreach(lib ${${item}_LIBRARIES})
				target_link_libraries(${this_target} PUBLIC ${lib})
			endforeach()
            if(NOT ${${item}_LIBRARIES} MATCHES ${item})
                message(WARNING "${item}_LIBRARIES '${${item}_LIBRARIES}' has no ${item}")
            endif()
        elseif(${item}_LIBRARY)
			dep(${${item}_LIBRARY})
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
	
	link_boost_libs()

    if(NOT MSVC)
        deps(
            pthread
			tbb
            )
    endif()

    if (OPENMIND_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
    elseif(OPENMIND_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Test)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Test)
    endif ()

	if(CPACK_BINARY_NSIS)
		LIST(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA "  CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${this_target}.lnk' '$INSTDIR\\\\bin\\\\${this_target}.exe' '' '$INSTDIR\\\\bin\\\\${this_target}.exe' 0 SW_SHOWNORMAL '' '%APPDATA%\\\\${this_target}'")
		LIST(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA "  Delete '$SMPROGRAMS\\\\$START_MENU\\\\${this_target}.lnk'")
		LIST(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA "  CreateShortCut '$DESKTOP\\\\${this_target}.lnk' '$INSTDIR\\\\bin\\\\${this_target}.exe' '' '$INSTDIR\\\\bin\\\\${this_target}.exe' 0 SW_SHOWNORMAL '' '%APPDATA%\\\\${this_target}'")
		LIST(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA "  Delete '$DESKTOP\\\\${this_target}.lnk'")
	endif()

endmacro()

macro(util)
	exe(${ARGN})
	set_target_properties(${this_target} PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util")
endmacro()
