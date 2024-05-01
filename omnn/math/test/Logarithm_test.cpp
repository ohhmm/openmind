#define BOOST_TEST_MODULE Logarithm test
#include <boost/test/unit_test.hpp>

#include "Exponentiation.h"

#include <cassert>
#include <iostream>

using namespace omnn::math;


// Test the optimize method for a logarithm where the target is a power of the base
BOOST_AUTO_TEST_CASE(Logarithm_power_of_base_optimization) {
    Valuable base = 2;
    Valuable target = Exponentiation{base, 3}; // 2^3
    Valuable log = Logarithm(base, target);
    log.optimize();
    BOOST_TEST(log == 3);
}

// Test the optimize method for a logarithm where the base and target are the same
BOOST_AUTO_TEST_CASE(Logarithm_base_equal_target_optimization) {
    Valuable base = 10;
    Valuable target = 10;
    Valuable log = Logarithm(base, target);
    log.optimize(); 
    BOOST_TEST(log == 1);
}
