# Add test executables
enable_testing()

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    # Ensure consistent runtime library settings
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Debug>:DLL>")
    # On Windows, we need to explicitly link against the Boost UTF DLL
    set(BOOST_TEST_LINK_LIBS Boost::unit_test_framework)
else()
    set(BOOST_TEST_LINK_LIBS Boost::unit_test_framework)
endif()

# Function to add test with common settings
function(add_test_executable TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME}
        PRIVATE
        math
        ${BOOST_TEST_LINK_LIBS}
        ${ARGN}
    )
    add_test(
        NAME ${TEST_NAME}
        COMMAND ${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_tests_properties(${TEST_NAME} PROPERTIES
        ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all"
        ENVIRONMENT "BOOST_TEST_COLOR_OUTPUT=1"
    )
endfunction()

# Add test executables with common dependencies
add_test_executable(ShorAlgorithm_test ShorAlgorithm_test.cpp
    Boost::serialization
    Boost::filesystem
    Boost::system
    Boost::thread
)

add_test_executable(ModularExponentiation_test ModularExponentiation_test.cpp)
add_test_executable(PeriodFinder_test PeriodFinder_test.cpp)
add_test_executable(QuantumFourierTransform_test QuantumFourierTransform_test.cpp)
add_test_executable(QuantumRegister_test QuantumRegister_test.cpp)

add_test_executable(Integer_test Integer_test.cpp
    Boost::serialization
    Boost::filesystem
    Boost::system
    Boost::thread
)

add_test_executable(Complex_test Complex_test.cpp)
