# Configure Boost test targets with consistent settings
function(test)
    if(NOT OPENMIND_BUILD_TESTS)
        return()
    endif()

    get_filename_component(test_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    file(GLOB_RECURSE TEST_SOURCES *.cpp *.h)

    add_executable(${test_name} ${TEST_SOURCES})

    if(MSVC)
        # Ensure consistent iterator debug level with Boost
        target_compile_definitions(${test_name} PRIVATE
            _ITERATOR_DEBUG_LEVEL=2
            _DEBUG_FUNCTIONAL_MACHINERY
        )
    endif()

    find_package(Boost REQUIRED COMPONENTS unit_test_framework)
    target_link_libraries(${test_name} PRIVATE
        ${this_target}
        Boost::unit_test_framework
    )

    add_test(NAME ${test_name} COMMAND ${test_name})
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 300)
endfunction()
