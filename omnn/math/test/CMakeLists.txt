if(MSVC)
    add_compile_options(/MT)
    set(Boost_USE_STATIC_RUNTIME ON)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
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
        ucrt
        msvcrt
    )
    target_compile_definitions(Fraction_test PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
    )
endif()

add_test(NAME Fraction_test COMMAND Fraction_test)
