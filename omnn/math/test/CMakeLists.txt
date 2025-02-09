if(MSVC)
    set(BOOST_TEST_LINK_LIBS
        Boost::serialization
        Boost::filesystem
        Boost::unit_test_framework
        Boost::test_exec_monitor
    )
    add_definitions(-DBOOST_TEST_DYN_LINK)
else()
    set(BOOST_TEST_LINK_LIBS
        Boost::serialization
        Boost::filesystem
        Boost::unit_test_framework
    )
endif()

test(math)
#target_link_libraries(08_System PUBLIC Boost::thread)
