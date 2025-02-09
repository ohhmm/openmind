#define BOOST_TEST_MODULE Geometric test
#include <boost/test/unit_test.hpp>

#include "pi.h"
#include "Variable.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(Semicircle_area_test) {
    // Calculate area of semicircle with radius 3
    // Area = 5 (given)
    Valuable radius = 3;
    Valuable area = (constants::pi * (radius ^ 2)) / constants::two;
    
    // Expected area = 5
    BOOST_TEST(area == 5);
}
