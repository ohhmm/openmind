#define BOOST_TEST_MODULE IsNegativeOf_comparator_test
#include <boost/test/unit_test.hpp>
#include "../../math/Integer.h"
#include "../../math/Variable.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(test_is_negative_of)
{
    Integer a(5);
    Integer b(10);
    Integer c(-3);
    Integer d(-7);
    Integer zero(0);

    // Test positive numbers
    BOOST_CHECK_EQUAL(a.IsNegativeOf(b), Integer(0));  // 5 is not -10
    BOOST_CHECK_EQUAL(b.IsNegativeOf(a), Integer(0));  // 10 is not -5

    // Test negative numbers
    BOOST_CHECK_EQUAL(c.IsNegativeOf(d), Integer(0));  // -3 is not -(-7)
    BOOST_CHECK_EQUAL(d.IsNegativeOf(c), Integer(0));  // -7 is not -(-3)

    // Test with zero
    BOOST_CHECK_EQUAL(zero.IsNegativeOf(zero), Integer(0));  // 0 is not -0
    BOOST_CHECK_EQUAL(a.IsNegativeOf(zero), Integer(0));     // 5 is not -0
    BOOST_CHECK_EQUAL(zero.IsNegativeOf(a), Integer(0));     // 0 is not -5

    // Test actual negative pairs
    Integer pos5(5);
    Integer neg5(-5);
    BOOST_CHECK_EQUAL(neg5.IsNegativeOf(pos5), Integer(1));   // -5 is negative of 5
    BOOST_CHECK_EQUAL(pos5.IsNegativeOf(neg5), Integer(0));  // 5 is not -(-5)
}
