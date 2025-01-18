find_package(Boost REQUIRED COMPONENTS unit_test_framework serialization filesystem)

# Create test targets with proper names
function(create_test_target)
    get_filename_component(TEST_NAME ${ARGV0} NAME_WE)
    add_executable(${TEST_NAME}_test ${ARGV0})
    target_link_libraries(${TEST_NAME}_test 
        PRIVATE 
        math
        Boost::unit_test_framework
    )
    target_compile_definitions(${TEST_NAME}_test PRIVATE
        TEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
        TEST_BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}/"
    )
endfunction()

# Add all test files
file(GLOB TEST_SOURCES "*.cpp")
foreach(TEST_SOURCE ${TEST_SOURCES})
    create_test_target(${TEST_SOURCE})
endforeach()

# Link Boost libraries for all test targets
get_property(TEST_TARGETS DIRECTORY PROPERTY TESTS)
foreach(TEST_TARGET ${TEST_TARGETS})
    target_link_libraries(${TEST_TARGET} 
        PUBLIC 
        ${BOOST_TEST_LINK_LIBS}
        math
    )
endforeach()
