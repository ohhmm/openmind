#include "../Exponentiation.h"
#include "../Integer.h"
#include "../Fraction.h"
#include <boost/test/unit_test.hpp>

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(test_one_base_integer_multiplication)
{
    // Test case for transforming n*(1^x) to (1/n)*(1^x)
    auto one = Valuable(1);
    auto half = Fraction(1, 2);
    auto five = Valuable(5);

    // Create expression 5*(1^(1/2))
    auto exp1 = one ^ half;
    auto result1 = exp1 * five;

    // Create expression (1/5)*(1^(1/2))
    auto exp2 = one ^ half;
    auto frac = Fraction(1, 5);
    auto result2 = exp2 * frac;

    // Verify both expressions optimize to the same form
    BOOST_CHECK_EQUAL(result1, result2);

    // Test with negative integer
    auto neg_five = Valuable(-5);
    auto result3 = exp1 * neg_five;
    auto neg_frac = Fraction(1, -5);
    auto result4 = exp2 * neg_frac;
    BOOST_CHECK_EQUAL(result3, result4);
}

BOOST_AUTO_TEST_CASE(test_one_base_complex_transformations)
{
    auto one = Valuable(1);
    auto third = Fraction(1, 3);
    auto two = Valuable(2);
    auto three = Valuable(3);

    // Test nested transformations
    auto exp1 = one ^ third;  // 1^(1/3)
    auto result1 = (exp1 * two) * three;  // (2*(1^(1/3)))*3

    // Compare with direct transformation
    auto exp2 = one ^ third;
    auto frac = Fraction(1, 6);  // 1/(2*3)
    auto result2 = exp2 * frac;

    BOOST_CHECK_EQUAL(result1, result2);

    // Test with fractions in exponent
    auto quarter = Fraction(1, 4);
    auto exp3 = one ^ quarter;
    auto result3 = exp3 * two;
    auto result4 = exp3 * Fraction(1, 2);
    BOOST_CHECK_EQUAL(result3, result4);

    // Test commutativity
    auto exp4 = one ^ half;
    auto result5 = two * exp4;  // Test multiplication from the other side
    auto result6 = exp4 * two;
    BOOST_CHECK_EQUAL(result5, result6);
}

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
    auto result4 = exp2 * two;
    auto result5 = exp2 * Fraction(1, 2);
    BOOST_CHECK_EQUAL(result4, result5);
}
