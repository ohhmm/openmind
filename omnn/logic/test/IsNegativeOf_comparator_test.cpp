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
    BOOST_TEST(a.IsNegativeOf(10) == 0); // 5 < 10
    BOOST_TEST(b.IsNegativeOf(5) != 0); // 10 is not < 5

    // Test negative numbers
    BOOST_TEST(c.IsNegativeOf(-7) != 0); // -3 is not < -7
    BOOST_TEST(Integer(-7).IsNegativeOf(-3) == 0); // -7 < -3

    // Test with zero
    BOOST_TEST(constants::zero.IsNegativeOf(0) != 0);  // 0 is not < -0
    BOOST_TEST(a.IsNegativeOf(0) != 0);               // 5 is not < -0
    BOOST_TEST(constants::zero.IsNegativeOf(5) == 0);// 0 < 5

    // Test actual negative pairs
    Integer pos5(5);
    Integer neg5(-5);
    BOOST_TEST(neg5.IsNegativeOf(pos5) == 0);  // -5 is negative of 5
    BOOST_TEST(pos5.IsNegativeOf(neg5) != 0); // 5 is not -(-5)
}
