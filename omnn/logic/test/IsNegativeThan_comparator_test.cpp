#define BOOST_TEST_MODULE IsNegativeThan_comparator_test
#include <boost/test/unit_test.hpp>
#include "../../math/Integer.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(test_is_negative_than)
{
    Integer a(5);
    Integer b(10);
    Integer c(-3);
    Integer d(-7);
    Integer zero(0);

    // Test positive numbers
    BOOST_CHECK(!a.IsNegativeThan(b));  // 5 is not -10
    BOOST_CHECK(!b.IsNegativeThan(a));  // 10 is not -5

    // Test negative numbers
    BOOST_CHECK(!c.IsNegativeThan(d));  // -3 is not -(-7)
    BOOST_CHECK(!d.IsNegativeThan(c));  // -7 is not -(-3)

    // Test with zero
    BOOST_CHECK(!zero.IsNegativeThan(zero));  // 0 is not -0
    BOOST_CHECK(!a.IsNegativeThan(zero));     // 5 is not -0
    BOOST_CHECK(!zero.IsNegativeThan(a));     // 0 is not -5

    // Test actual negative pairs
    Integer pos5(5);
    Integer neg5(-5);
    BOOST_CHECK(neg5.IsNegativeThan(pos5));   // -5 is -5
    BOOST_CHECK(!pos5.IsNegativeThan(neg5));  // 5 is not -(-5)
}
