#define BOOST_TEST_MODULE Less_comparator_test
#include <boost/test/unit_test.hpp>
#include "../../math/Integer.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(test_less_comparator)
{
    Integer a(5);
    Integer b(10);
    Integer c(-3);
    Integer d(-7);

    // Test positive numbers
    BOOST_CHECK(a < b);
    BOOST_CHECK(!(b < a));
    BOOST_CHECK(!(a < a));

    // Test negative numbers
    BOOST_CHECK(d < c);
    BOOST_CHECK(!(c < d));

    // Test mixed positive and negative
    BOOST_CHECK(c < a);
    BOOST_CHECK(d < b);
    BOOST_CHECK(!(a < c));
    BOOST_CHECK(!(b < d));

    // Test with zero
    Integer zero(0);
    BOOST_CHECK(c < zero);
    BOOST_CHECK(d < zero);
    BOOST_CHECK(!(zero < zero));
    BOOST_CHECK(!(a < zero));
    BOOST_CHECK(!(b < zero));
    BOOST_CHECK(zero < a);
    BOOST_CHECK(zero < b);
}
