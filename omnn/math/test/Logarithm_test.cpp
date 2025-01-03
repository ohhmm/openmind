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

// Test string parsing for logarithm expressions
BOOST_AUTO_TEST_CASE(Logarithm_string_parse_test) {
    // Test basic numeric case
    Valuable v("log(2,8)");
    v.optimize();
    BOOST_TEST(v == 3);

    // Test with spaces
    v = Valuable("log( 2 , 8 )");
    v.optimize();
    BOOST_TEST(v == 3);

    // Test with variables
    v = Valuable("log(x, x^2)");
    BOOST_TEST(v.IsLogarithm());
    auto& log = v.as<Logarithm>();
    BOOST_TEST(log.getBase().str() == "x");
    BOOST_TEST(log.getTarget().str() == "(x^2)");

    // Test with expressions
    v = Valuable("log(2+1, (2+1)^2)");
    v.optimize();
    BOOST_TEST(v == 2);
}
