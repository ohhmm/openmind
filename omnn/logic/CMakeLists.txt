add_library(devin_logic
    DevinLogicInterface.cpp
    Equality.cpp)

target_include_directories(devin_logic
    PUBLIC
    ${CMAKE_SOURCE_DIR})

target_link_libraries(devin_logic
    PUBLIC
    math)

if(OPENMIND_BUILD_TESTS)
    add_executable(DevinLogicInterface_test
        test/DevinLogicInterface_test.cpp)
    target_link_libraries(DevinLogicInterface_test
        PRIVATE
        devin_logic
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    add_test(NAME DevinLogicInterface_test 
             COMMAND DevinLogicInterface_test)
endif()
