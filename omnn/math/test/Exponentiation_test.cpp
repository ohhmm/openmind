#define BOOST_TEST_MODULE Exponentiation test
#include <boost/test/unit_test.hpp>

#include "Variable.h"

BOOST_AUTO_TEST_CASE(Exponentiation_tests)
{
    auto a = 2_v ^ 3;
    BOOST_TEST(a==8);
    a = 2_v ^ 11;
    BOOST_TEST(a==(1<<11));
    omnn::math::Variable x;
    auto _1 = (x-2)^3;
    auto _2 = (x-2)*(x-2)*(x-2);
    BOOST_TEST(_1==_2);
    _1 = -32;
    _1 ^= .5;
    _2 = 4;
    _2 *= -2_v^(1_v/2);
    BOOST_TEST(_1==_2);
    _1 = -1_v*((-4_v)^((1_v/2)))*((-16_v)^((1_v/2)));
    _2 = 8;
    BOOST_TEST(_1==_2);
    
    _1 = (((x-2)^2)+((x+2)^2));
    _1 = _1(x);
    _2 = ((-1)^(1_v/2))*2;
    BOOST_TEST(_1==_2);
}
