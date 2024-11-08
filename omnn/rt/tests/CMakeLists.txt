# Set Boost dynamic linking
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED COMPONENTS
    unit_test_framework
    chrono
    filesystem
    serialization
    system
    thread
    test_exec_monitor
)

# Add the fastest_get test
add_executable(fastest_get fastest_get.cpp)
target_link_libraries(fastest_get
    PRIVATE
    rt
    Boost::unit_test_framework
    Boost::chrono
    Boost::filesystem
    Boost::serialization
    Boost::system
    Boost::thread
    Boost::test_exec_monitor
)

# Add compile definitions for dynamic linking
target_compile_definitions(fastest_get
    PRIVATE
    BOOST_TEST_DYN_LINK
    BOOST_ALL_DYN_LINK
)

# Add test to CTest
add_test(NAME fastest_get COMMAND fastest_get)
