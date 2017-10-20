#define BOOST_TEST_MODULE Exponentiation test
#include <boost/test/unit_test.hpp>

#include "Exponentiation.h"

BOOST_AUTO_TEST_CASE(Exponentiation_tests)
{
    auto a = 2 ^ 3;
    BOOST_TEST(a==8);
	// todo : test exponentiation
}
