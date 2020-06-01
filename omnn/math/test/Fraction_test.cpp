#define BOOST_TEST_MODULE Fraction test
#include <boost/test/unit_test.hpp>

#include "Fraction.h"

using namespace omnn::math;
using namespace boost::unit_test_framework;

std::string l(const omnn::math::Valuable& v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

BOOST_AUTO_TEST_CASE(Fraction_tests)
{
    const auto a = 1_v / 2;
	auto c = 3_v / 1;
    auto b = a * 4;
	auto d = 2_v / 4;
    
    BOOST_TEST(a*b==1);
	BOOST_TEST((c += b) == 5);
	BOOST_TEST((c *= a) == 5_v / 2);
	BOOST_TEST((c /= a) == 5);
	BOOST_TEST((c--) == 5);
	BOOST_TEST((c++) == 4);
	BOOST_TEST(c > a);
	BOOST_TEST(a == d);
	BOOST_TEST(a - d == 0);

    Valuable _ = 3.1_v;
    BOOST_TEST(Fraction::cast(_));

    _ = (1_v/2)^2_v;
    BOOST_TEST(_ == 1_v/4);
    
    Variable v1, v2;
    _ = 1_v / (1_v / v1);
    BOOST_TEST(_ == v1);
    
    BOOST_TEST((2040_v*v1/(-2_v*v1))==-1020);
    
    _ = (2040_v/v1) / ((-1_v/v1)*v2);
    _.optimize();
    BOOST_TEST(_ == -2040_v/v2);
    
    BOOST_TEST((Fraction{1,-2}).operator<(0));

    _ = 1_v^(1_v/2);
    auto eq = _ == (1_v^(1_v/2));
    BOOST_TEST(eq);
    BOOST_TEST((_.IsMultival() == Valuable::YesNoMaybe::Yes));
    _ /= 1_v^(1_v/2);
    BOOST_TEST((_.IsMultival() == Valuable::YesNoMaybe::Yes));
    eq = _ == (1_v^(1_v/2));
    BOOST_TEST(eq);
}

BOOST_AUTO_TEST_CASE(Fraction_with_sum_tests
                     ,*disabled()
                     )
{
    auto _ = 841_v/64;
    _ ^= 1_v/2;
    auto a = (573440_v*(((841_v/64))^((1_v/2))) + 2115584)/262144;
    a.optimize();
    
    for (int i=38; i --> 1; ) {
        Valuable sh(1<<i);
        auto multi = 1_v^(1_v/sh);
        _ = multi;
        _ /= _;
        BOOST_TEST(_ == multi);
    }
}
