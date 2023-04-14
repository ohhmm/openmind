include(deps)
include(fold)
include(qtect)


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
	if(OPENMIND_USE_OPENCL)
		target_compile_definitions(${this_target} PUBLIC OPENMIND_USE_OPENCL)
		if(OPENMIND_DEBUG_CHECKS)
			target_compile_definitions(${this_target} PUBLIC BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION)
		endif()
	endif()
	if(NOT OPENMIND_DEBUG_CHECKS)
		target_compile_definitions(${this_target} PUBLIC NOOMDEBUG)
	endif()
	set_target_properties(${this_target} PROPERTIES
		CXX_STANDARD 20
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
		EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin
		)
	if(MSVC)
		target_compile_definitions(${this_target} PUBLIC
			APPLE_CONSTEXPR=
			MSVC_CONSTEXPR=constexpr
			NO_MSVC_CONSTEXPR=
			MSVC
			$<$<CONFIG:DEBUG>:BOOST_ALL_STATIC_LINK>
			$<$<CONFIG:Release>:BOOST_ALL_STATIC_LINK>
			$<$<CONFIG:RelWithDebInfo>:BOOST_ALL_STATIC_LINK>
			$<$<CONFIG:MinSizeRel>:BOOST_ALL_STATIC_LINK>
			)
		target_compile_options(${this_target} PUBLIC
			/bigobj
			/FS
			/MP
			/constexpr:steps1000000000
			/source-charset:utf-8
			$<$<CONFIG:DEBUG>:/ZI>
			$<$<CONFIG:Release>:/MT>
			$<$<CONFIG:RelWithDebInfo>:/ZI>
			)
		target_link_options(${this_target} PUBLIC
			$<$<CONFIG:DEBUG>:/LTCG:OFF>
			$<$<CONFIG:RelWithDebInfo>:/LTCG:OFF>
			)
		message("${CMAKE_BINARY_DIR}/bin")
	    target_link_directories(${this_target} PUBLIC
			${CMAKE_BINARY_DIR}/bin
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
	get_filename_component(parent_target ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
	get_filename_component(parent_target ${parent_target} NAME)
	project(${parent_target})
	message("\nCreating Tests for ${parent_target}")
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

		set(this_target ${TEST_NAME})
		apply_target_commons(${TEST_NAME})
		if(NOT MSVC AND Boost_FOUND)
			deps(${BOOST_TEST_LINK_LIBS})
		endif()
		deps(${libs})

		set_target_properties(${TEST_NAME} PROPERTIES
			FOLDER "test"
		)
		target_compile_definitions(${TEST_NAME} PUBLIC
			-DTEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
			-DTEST_BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}/"
			)
		add_dependencies(${TEST_NAME} ${parent_target})
		#add_dependencies(${TEST_NAME} prerequisites)

		target_link_directories(${TEST_NAME} PUBLIC
			/usr/local/lib
			${Boost_INCLUDE_DIR}/stage/lib
			${Boost_INCLUDE_DIR}/../../lib
			${EXTERNAL_FETCHED_BOOST}/stage/lib
			${EXTERNAL_FETCHED_BOOST}/../../lib
			${CMAKE_BINARY_DIR}/lib
			${CMAKE_BINARY_DIR}/lib64
			)
        target_link_libraries(${TEST_NAME} PUBLIC ${parent_target})

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
    #add_dependencies(${this_target} prerequisites)
	message("add_library(${this_target} ${src})")
	if(NOT MSVC AND Boost_FOUND)
		foreach (boostlibtarget ${BOOST_LINK_LIBS})
			if(TARGET ${boostlibtarget})
				message("linking boostlibtarget: ${boostlibtarget}")
				target_link_libraries(${this_target} PUBLIC ${boostlibtarget})
			else()
				message("skipping linking ${boostlibtarget}, the target not found")
			endif()
		endforeach()
		foreach (boostcomponent ${BOOST_ADDITIONAL_COMPONENTS})
			string(TOUPPER ${boostcomponent} boostcomponent)
			message("include ${boostcomponent}: ${Boost_${boostcomponent}_INCLUDE_DIR}")
			target_link_libraries(${this_target} PUBLIC ${Boost_${boostcomponent}_INCLUDE_DIR})
			message("linking ${boostcomponent}: ${Boost_${boostcomponent}_LIBRARY}")
			target_link_libraries(${this_target} PUBLIC ${Boost_${boostcomponent}_LIBRARY})
		endforeach()
	endif()
	#message("target_link_libraries(${this_target} PUBLIC ${deps})")
    deps(${TEST_DEPS} ${deps})

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

macro(exe)
    string(STRIP "${ARGN}" deps)

    get_filename_component(this_target ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	unset(this_target_n)
	while(TARGET ${this_target}${this_target_n})
		math(EXPR this_target_n "${this_target_n} + 1")
	endwhile()
	set(this_target ${this_target}${this_target_n})

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
    #add_dependencies(${this_target} prerequisites)
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
                deps(${${item}_LIBRARIES})
                if(NOT "${${item}_LIBRARIES}" MATCHES ${item})
                    message(WARNING "${item}_LIBRARIES '${${item}_LIBRARIES}' has no ${item}")
                endif()
            endif()
        elseif(${item}_LIBRARIES)
            message("Linking ${item}_LIBRARIES ${${item}_LIBRARIES}")
			deps(${${item}_LIBRARIES})
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
    if(NOT MSVC)
        target_link_libraries(${this_target} PUBLIC
            pthread
            ${BOOST_LINK_LIBS}
            )
		if(OPENMIND_USE_TBB)
			target_link_libraries(${this_target} PUBLIC TBB::tbb)
		endif()
    endif()

    if (OPENMIND_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
    elseif(OPENMIND_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Test)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Test)
    endif ()

endmacro()
