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
