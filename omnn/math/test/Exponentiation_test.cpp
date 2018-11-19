#define BOOST_TEST_MODULE Exponentiation test
#include <boost/test/unit_test.hpp>

#include "Variable.h"

using namespace omnn::math;
using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(Exponentiation_tests)
{
    auto a = 2_v ^ 3;
    BOOST_TEST(a==8);
    a = 2_v ^ 11;
    BOOST_TEST(a==(1<<11));
    Variable x;
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

BOOST_AUTO_TEST_CASE(Sqrt_test)
{
    auto a = 25_v;
    auto e = a ^ (1_v/2);
    BOOST_TEST(e == (1_v^(1_v/2))*5);
    BOOST_TEST(e != 5);
    // https://math.stackexchange.com/questions/41784/convert-any-number-to-positive-how/41787#comment5776496_41787
    e = a.Sqrt();
    BOOST_TEST(e == 5);
}

BOOST_AUTO_TEST_CASE(Polynomial_Sqrt_test
                     ,*disabled()
                     )
{
    Variable x,y;
    auto a = 4*(x^2)-12*x*y+9*(y^2);
    auto e = a.Sqrt();
    BOOST_TEST(e == 2*x-3*y);
}
