#define BOOST_TEST_MODULE IntegerExponentiation test
#include <boost/test/unit_test.hpp>

#include "Exponentiation.h"
#include "Integer.h"
#include "Fraction.h"
#include "Variable.h"
#include "Sum.h"

using namespace boost::unit_test;
using namespace omnn::math;


BOOST_AUTO_TEST_CASE(test_one_base_edge_cases)
{
    auto one = Valuable(1);
    auto half = Fraction(1, 2);

    // Test with 1 as multiplier
    auto exp1 = one ^ half;
    auto result1 = exp1 * one;
    BOOST_CHECK_EQUAL(result1, exp1);

    // Test with -1 as multiplier
    auto neg_one = Valuable(-1);
    auto result2 = exp1 * neg_one;
    auto result3 = exp1 * Fraction(1, -1);
    BOOST_CHECK_EQUAL(result2, result3);

    // Test with zero exponent
    auto zero = Valuable(0);
    auto exp2 = one ^ zero;
    auto result4 = exp2 * constants::two;
    auto result5 = exp2 * Fraction(1, 2);
    BOOST_CHECK_EQUAL(result4, result5);
}
