# Set Boost dynamic linking
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED COMPONENTS
    unit_test_framework
    filesystem
    system
    serialization
)

# Add the LevelDbStorage test
add_executable(LevelDbStorage LevelDbStorage.cpp)
target_link_libraries(LevelDbStorage
    PRIVATE
    storage
    Boost::unit_test_framework
    Boost::filesystem
    Boost::system
    Boost::serialization
)

# Add compile definitions for dynamic linking
target_compile_definitions(LevelDbStorage
    PRIVATE
    BOOST_TEST_DYN_LINK
    BOOST_ALL_DYN_LINK
)

# Add test to CTest
add_test(NAME LevelDbStorage COMMAND LevelDbStorage)
