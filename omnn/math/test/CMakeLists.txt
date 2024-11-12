cmake
cmake_minimum_required(VERSION 3.10)

if(MSVC)
    # Use the global runtime library and iterator debug level settings
    # No need to set _ITERATOR_DEBUG_LEVEL here as it's handled in the root CMakeLists.txt

    # Boost configuration
    set(Boost_USE_STATIC_LIBS ON)
    add_definitions(-D_MT)
endif()

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
    target_link_libraries(Fraction_test PRIVATE
        legacy_stdio_definitions
    )

    # Debug-specific libraries using generator expressions
    target_link_libraries(Fraction_test PRIVATE
        $<$<CONFIG:Debug>:ucrtd>
        $<$<CONFIG:Debug>:vcruntimed>
        $<$<CONFIG:Debug>:msvcrtd>
    )

    target_compile_definitions(Fraction_test PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        $<$<CONFIG:Debug>:_DEBUG>
    )
endif()

add_test(NAME Fraction_test COMMAND Fraction_test)
