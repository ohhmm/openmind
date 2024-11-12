# Configure Boost test targets with consistent settings
function(test)
    if(NOT OPENMIND_BUILD_TESTS)
        return()
    endif()

    get_filename_component(test_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    file(GLOB_RECURSE TEST_SOURCES *.cpp *.h)

    # Append "_test" to make target name unique and not conflict with CTest's reserved "test" target
    set(target_name "${test_name}_test")
    add_executable(${target_name} ${TEST_SOURCES})

    if(MSVC)
        # Ensure consistent iterator debug level with Boost
        add_definitions(-D_ITERATOR_DEBUG_LEVEL=2)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_ITERATOR_DEBUG_LEVEL=2")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D_ITERATOR_DEBUG_LEVEL=2")

        target_compile_definitions(${target_name} PRIVATE
            _ITERATOR_DEBUG_LEVEL=2
            _DEBUG_FUNCTIONAL_MACHINERY
        )

        # Propagate iterator debug level to dependencies
        get_target_property(test_deps ${target_name} LINK_LIBRARIES)
        if(test_deps)
            foreach(dep ${test_deps})
                if(TARGET ${dep})
                    target_compile_definitions(${dep} PRIVATE
                        _ITERATOR_DEBUG_LEVEL=2
                        _DEBUG_FUNCTIONAL_MACHINERY
                    )
                endif()
            endforeach()
        endif()
    endif()

    find_package(Boost REQUIRED COMPONENTS unit_test_framework)
    target_link_libraries(${target_name} PRIVATE
        ${this_target}
        Boost::unit_test_framework
    )

    add_test(NAME ${test_name} COMMAND ${target_name})
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 300)
endfunction()
