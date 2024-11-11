find_package(Boost REQUIRED COMPONENTS
    unit_test_framework
    filesystem
    serialization
)

add_executable(Fraction_test Fraction_test.cpp)
target_link_libraries(Fraction_test
    PUBLIC
    math
    Boost::unit_test_framework
    Boost::filesystem
    Boost::serialization
)
add_test(NAME Fraction_test COMMAND Fraction_test)
