# Add test executables
add_executable(ShorAlgorithm_test ShorAlgorithm_test.cpp)
target_link_libraries(ShorAlgorithm_test
    PRIVATE
    math
    Boost::unit_test_framework
    Boost::serialization
    Boost::filesystem
    Boost::system
    Boost::thread
)

add_executable(ModularExponentiation_test ModularExponentiation_test.cpp)
target_link_libraries(ModularExponentiation_test
    PRIVATE
    math
    Boost::unit_test_framework
)

add_executable(PeriodFinder_test PeriodFinder_test.cpp)
target_link_libraries(PeriodFinder_test
    PRIVATE
    math
    Boost::unit_test_framework
)

add_executable(QuantumFourierTransform_test QuantumFourierTransform_test.cpp)
target_link_libraries(QuantumFourierTransform_test
    PRIVATE
    math
    Boost::unit_test_framework
)

add_executable(QuantumRegister_test QuantumRegister_test.cpp)
target_link_libraries(QuantumRegister_test
    PRIVATE
    math
    Boost::unit_test_framework
)

add_executable(Integer_test Integer_test.cpp)
target_link_libraries(Integer_test
    PRIVATE
    math
    Boost::unit_test_framework
    Boost::serialization
    Boost::filesystem
    Boost::system
    Boost::thread
)

# Add tests to CTest
add_test(NAME ShorAlgorithm_test COMMAND ShorAlgorithm_test)
add_test(NAME ModularExponentiation_test COMMAND ModularExponentiation_test)
add_test(NAME PeriodFinder_test COMMAND PeriodFinder_test)
add_test(NAME QuantumFourierTransform_test COMMAND QuantumFourierTransform_test)
add_test(NAME QuantumRegister_test COMMAND QuantumRegister_test)
add_test(NAME Integer_test COMMAND Integer_test)
