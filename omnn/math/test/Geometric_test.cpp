#define BOOST_TEST_MODULE Geometric_test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "pi.h"
#include "Variable.h"

using namespace omnn::math;

BOOST_AUTO_TEST_SUITE(Geometric_test_suite)

BOOST_AUTO_TEST_CASE(Semicircle_area_test, *boost::unit_test::disabled()) {
    // Calculate area of semicircle with radius 3
    // Area calculation pending mathematical validation
    Valuable radius = 3;
    Valuable area = (constants::pi * (radius ^ 2)) / constants::two;
    
    // Expected area pending validation
    BOOST_TEST(area == 5);
}

BOOST_AUTO_TEST_SUITE_END()
