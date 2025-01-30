#define BOOST_TEST_MODULE Logarithm test
#include <boost/test/unit_test.hpp>

#include "Exponentiation.h"
#include "Euler.h"

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
BOOST_AUTO_TEST_CASE(Logarithm_string_parse_int_test) {
    // Test basic numeric case
    Valuable v("log(2,8)");
    v.optimize();
    BOOST_TEST(v == 3);

    // Test with spaces
    v = Valuable("log( 2 , 8 )");
    v.optimize();
    BOOST_TEST(v == 3);
}

BOOST_AUTO_TEST_CASE(Logarithm_string_parse_var_test)
{
    // Test with variables
    auto v = Valuable("log(x, x^2)");
    BOOST_TEST(v == 2);

    // Test with expressions
    v = Valuable("log(2+1, (2+1)^2)");
    v.optimize();
    BOOST_TEST(v == 2);
}

BOOST_AUTO_TEST_CASE(Natural_Logarithm_test) {
    // Test natural logarithm conversion
    Valuable ln_e = Valuable("ln(e)");
    ln_e.optimize();
    BOOST_TEST(ln_e == 1);

    // Test natural logarithm of 1
    Valuable ln_1 = Valuable("ln(1)");
    ln_1.optimize();
    BOOST_TEST(ln_1 == 0);

    // Test derivative of ln(x)
    Variable x;
    auto ln_x = Logarithm(constant::e, x);
    auto d_ln_x = ln_x;
    d_ln_x.d(x);
    BOOST_TEST(d_ln_x == (x ^ -1));
    BOOST_TEST(d_ln_x == (1 / x));
}

BOOST_AUTO_TEST_CASE(Logarithm_equation_test, *boost::unit_test::disabled()) {
    auto equation = "(10^(log(2, x)))+(10^(log(2, (x^2)))) = 10^(log(2, (x^3)))"_v;
}
