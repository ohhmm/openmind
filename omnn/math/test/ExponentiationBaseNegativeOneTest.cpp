#include "Exponentiation.h"
#include "Integer.h"
#include <boost/test/unit_test.hpp>

using namespace omnn::math;

BOOST_AUTO_TEST_SUITE(ExponentiationBaseNegativeOneTest)

BOOST_AUTO_TEST_CASE(test_negative_one_base_even_exponent)
{
    // Test (-1)^2 = 1
    auto base = Integer(-1);
    auto exp = Integer(2);
    auto result = base ^ exp;
    BOOST_CHECK_EQUAL(result, Integer(1));

    // Test (-1)^4 = 1
    exp = Integer(4);
    result = base ^ exp;
    BOOST_CHECK_EQUAL(result, Integer(1));
}

BOOST_AUTO_TEST_CASE(test_negative_one_base_odd_exponent)
{
    // Test (-1)^3 = -1
    auto base = Integer(-1);
    auto exp = Integer(3);
    auto result = base ^ exp;
    BOOST_CHECK_EQUAL(result, Integer(-1));

    // Test (-1)^5 = -1
    exp = Integer(5);
    result = base ^ exp;
    BOOST_CHECK_EQUAL(result, Integer(-1));
}

BOOST_AUTO_TEST_SUITE_END()
