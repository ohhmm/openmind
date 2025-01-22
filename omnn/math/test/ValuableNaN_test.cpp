#define BOOST_TEST_MODULE ValuableNaN_test
#include <boost/test/unit_test.hpp>
#include "omnn/math/Valuable.h"
#include "omnn/math/NaN.h"
#include "omnn/math/Exponentiation.h"

using namespace omnn::math;

BOOST_AUTO_TEST_SUITE(ValuableNaN_test)

BOOST_AUTO_TEST_CASE(NaNComparisonTests)
{
    // Test NaN comparison behavior according to IEEE 754-2019 section 6.2.1
    Valuable nan = constant::nan;
    Valuable one(1);
    Valuable zero(0);
    
    // Verify NaN comparisons always return false
    BOOST_CHECK((nan < one) == false);
    BOOST_CHECK((one < nan) == false);
    BOOST_CHECK((nan == nan) == false);
    BOOST_CHECK((nan == one) == false);
    
    // Test 0^0 case and its NaN behavior
    Valuable zeroExpZero = zero ^ zero;
    BOOST_CHECK(zeroExpZero.IsNaN());
    
    // Verify that 0^0 as NaN follows IEEE 754 comparison rules
    BOOST_CHECK((zeroExpZero < one) == false);
    BOOST_CHECK((one < zeroExpZero) == false);
    BOOST_CHECK((zeroExpZero == zeroExpZero) == false);
    
    // Test NaN propagation in expressions
    Valuable expr = (zero ^ zero) + one;
    BOOST_CHECK(expr.IsNaN());
    BOOST_CHECK((expr == expr) == false);
}

BOOST_AUTO_TEST_CASE(NaNOrderingConsistency)
{
    // Test that NaN maintains consistent ordering in expression trees
    Valuable nan = constant::nan;
    Valuable one(1);
    Valuable minusOne(-1);
    
    // NaN should not be ordered relative to any value
    BOOST_CHECK((nan < minusOne) == false);
    BOOST_CHECK((minusOne < nan) == false);
    BOOST_CHECK((nan < one) == false);
    BOOST_CHECK((one < nan) == false);
    
    // NaN should not equal itself
    BOOST_CHECK((nan == nan) == false);
    
    // Different NaN sources should maintain consistent non-ordering
    Valuable nanFromZeroDiv = one / Valuable(0);
    BOOST_CHECK((nanFromZeroDiv < nan) == false);
    BOOST_CHECK((nan < nanFromZeroDiv) == false);
    BOOST_CHECK((nanFromZeroDiv == nan) == false);
}

BOOST_AUTO_TEST_SUITE_END()
