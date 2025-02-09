set(BOOST_TEST_LINK_LIBS
    Boost::serialization
    Boost::filesystem
    Boost::unit_test_framework
)
test(math)
#target_link_libraries(08_System PUBLIC Boost::thread)
