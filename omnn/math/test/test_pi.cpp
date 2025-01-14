#define BOOST_TEST_MODULE Pi test
#include <boost/test/unit_test.hpp>

#include "pi.h"
#include "Variable.h"
#include "Sum.h"
#include "generic.hpp"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(test_pi_comparison) {
    Pi pi;
    
    // Test self-comparison
    BOOST_TEST((pi < pi) == false);
    
    // Test comparison with positive numbers
    BOOST_TEST((pi < 4) == true);
    BOOST_TEST((pi < 3) == false);
    
    // Test comparison with negative numbers
    BOOST_TEST((pi < -1) == false);
    BOOST_TEST((pi < -5) == false);
    BOOST_TEST((pi < -10) == false);
    
    // Test comparison with zero
    BOOST_TEST((pi < 0) == false);
    
    // Test comparison with equal negative numbers
    Valuable v(-10);
    BOOST_TEST((pi < v) == false);
    v = Valuable(-10);
    BOOST_TEST((pi < v) == false);
}
