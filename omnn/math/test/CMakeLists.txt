set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)

test()

# Link test targets with required libraries
foreach(test_target
    08_System
    8Q
    ActivationFunctionTests
    Exponentiation_test
    Fraction_test
    IntegerExponentiation_test
    Integer_test
    Logarithm_test
    Modulo_test
    MultiValues_test
    Polyfit_test
    Product_test
    Radicals
    SchoolTasks
    Stasis
    Sum_test
    SySHA_test
    TicTacToe
    VarHost_test
    angle_solvability_test
    differ
    logic
    serialization
    sh_test
    ts)
    if(WIN32)
        find_package(leveldb CONFIG REQUIRED)
        target_link_libraries(${test_target} PRIVATE leveldb::leveldb)
    else()
        find_package(leveldb REQUIRED)
        target_link_libraries(${test_target} PRIVATE leveldb)
    endif()
    target_link_libraries(${test_target} PRIVATE ${Boost_LIBRARIES})
endforeach()

# Link test executables with leveldb
foreach(test_target
    08_System
    8Q
    ActivationFunctionTests
    Exponentiation_test
    Fraction_test
    IntegerExponentiation_test
    Integer_test
    Logarithm_test
    Modulo_test
    MultiValues_test
    Polyfit_test
    Product_test
    Radicals
    SchoolTasks
    Stasis
    Sum_test
    SySHA_test
    TicTacToe
    VarHost_test
    angle_solvability_test
    differ
    logic
    serialization
    sh_test
    ts)
    if(WIN32)
        target_link_libraries(${test_target} PUBLIC leveldb::leveldb)
    else()
        target_link_libraries(${test_target} PUBLIC leveldb)
    endif()
endforeach()

# Link test executables with Boost test framework
foreach(test_target
    08_System
    8Q
    ActivationFunctionTests
    Exponentiation_test
    Fraction_test
    IntegerExponentiation_test
    Integer_test
    Logarithm_test
    Modulo_test
    MultiValues_test
    Polyfit_test
    Product_test
    Radicals
    SchoolTasks
    Stasis
    Sum_test
    SySHA_test
    TicTacToe
    VarHost_test
    angle_solvability_test
    differ
    logic
    serialization
    sh_test
    ts)
    target_link_libraries(${test_target} PUBLIC ${BOOST_TEST_LINK_LIBS})
endforeach()
