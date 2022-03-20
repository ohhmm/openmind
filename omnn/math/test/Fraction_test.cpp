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
    BOOST_TEST(_.IsFraction());

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

/// <summary>
/// See for some reference and description on multivalues origins
/// </summary>
BOOST_AUTO_TEST_CASE(Fraction_with_sum_tests) {
    auto _ = 841_v/64; // 841/64 = 13.140625
    auto a = _.Sqrt(); // 29/8 = 3.625   - square root stands for principal root (only positive branch).
	// Branch reduction leads to backward-incompatible expressions which leads to calculation misstakes which is why this framework has limited support for principal root

	// Multivalue expression equivalent:
    _ ^= 1_v/2; // (841/64)^(1/2) = ?.. lets see possible ways to calculate (covering multivals)
	// ±29/±8 = ±(29/8) = (1^(1/2))*(29/8) :   
	// 
	//  29^2  /  8^2  = 841/64
    //  29^2 / (-8)^2 = 841/64
    // (-29)^2 / 8^2  = 841/64
    // (-29)^2 / (-8)^2 = 841/64
	//                 <=>
	//	                  ___    - (841/64)^(1/2) equals some of these values and we do not know which of these exactly for current expression
	//  (841/64)^(1/2) = | 29/8 = -29/-8 = 3.625
	//                   | -29/8 = 29/-8 = -3.625
	//                   \___
	// 
	// NOTE: it does not equal to any of these particular values, but it equals this uncertinty, which of these two the value is
	//       probably the most suitable is to call it the set of these two values:
	//       (841/64)^(1/2) = ±29/±8 = ±(29/8) = set(-3.625, 3.625)
	//       mathematical unordered set of values
	//       multivalue
	//       this way there is not branch reduction and expressions are backward-compatibly equivalent
	//   ___                                  ___
    //  |  29^2  /  8^2  = 841/64            | (29/8)^2 = 841/64              ___
    //  |  29^2 / (-8)^2 = 841/64            | (29/-8)^2 = 841/64            | 29/8 = -29/-8 = 3.625
    //  | (-29)^2 / 8^2  = 841/64     <===>  | (-29/8)^2 = 841/64    <===> 	 | -29/8 = 29/-8 = -3.625    <===> (841/64)^(1/2) = ±29/±8 = ±(29/8) = set(-3.625, 3.625)
    //  | (-29)^2 / (-8)^2 = 841/64          | (-29/-8)^2 = 841/64			 \___
	//   \__                                  \__




    a = (573440_v*(((841_v/64))^((1_v/2))) + 2115584)/262144;
    // (573440 * (±29/±8) + 2115584) / 262144
    a.optimize();
    auto ok = a.IsMultival() == Valuable::YesNoMaybe::Yes;
    BOOST_TEST(ok);
    
    for (int64_t i=8; i --> 1; ) {
        Valuable sh(int64_t(1)<<i);
        auto multi = 1_v^(1_v/sh);
        _ = multi;
        _ /= _;
        BOOST_TEST(_ == multi);
    }
}
