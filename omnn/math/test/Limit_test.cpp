#define BOOST_TEST_MODULE Limit test
#include <boost/test/unit_test.hpp>

#include "Limit.h"
#include "Fraction.h"
#include "Exponentiation.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(Limit_basic_test, *boost::unit_test::disabled()) {
    // Test basic limit evaluation
    auto v = Valuable("lim(x->0, ((x^2)-4)/((x^2)+2*x-6))");
    v.optimize();
    BOOST_TEST(v == Fraction(4, 5));
}

BOOST_AUTO_TEST_CASE(Limit_indeterminate_form_test, *boost::unit_test::disabled()) {
    // Test 0/0 indeterminate form
    auto v = Valuable("lim(x->0, x/x)");
    v.optimize();
    BOOST_TEST(v == 1);
}

BOOST_AUTO_TEST_CASE(Limit_infinity_form_test, *boost::unit_test::disabled()) {
    // Test infinity/infinity form
    auto v = Valuable("lim(x->inf, (x^2+x)/(x^2-x))");
    v.optimize();
    BOOST_TEST(v == 1);
}

BOOST_AUTO_TEST_CASE(Limit_derivative_test, *boost::unit_test::disabled()) {
    // Test limit of derivative
    auto v = Valuable("lim(x->0, (sin(x))/x)");
    v.optimize();
    BOOST_TEST(v == 1);
}
