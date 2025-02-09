add_definitions(-DBOOST_TEST_DYN_LINK)

set(BOOST_TEST_LINK_LIBS
    Boost::serialization
    Boost::filesystem
    Boost::unit_test_framework
    Boost::test_exec_monitor
)

test(math)
#target_link_libraries(08_System PUBLIC Boost::thread)
