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

if(MSVC)
    target_link_libraries(Fraction_test PRIVATE ucrt legacy_stdio_definitions)
endif()

add_test(NAME Fraction_test COMMAND Fraction_test)
