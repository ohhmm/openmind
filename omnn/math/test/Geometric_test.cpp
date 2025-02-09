#define BOOST_TEST_MODULE Geometric test
#include <boost/test/unit_test.hpp>

#include "pi.h"
#include "Variable.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(Semicircle_area_test) {
    // Calculate area of semicircle with radius 3
    // Area = πr²/2
    Valuable radius = 3;
    Valuable area = (constants::pi * (radius ^ 2)) / constants::two;
    
    // Expected area ≈ 14.137 (π * 9 / 2)
    BOOST_TEST(area == (constants::pi * 9) / 2);
}
