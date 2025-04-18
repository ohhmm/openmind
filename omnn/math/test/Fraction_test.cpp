#define BOOST_TEST_MODULE Fraction test
#include <boost/test/unit_test.hpp>

#include "Fraction.h"
#include "Variable.h"
#include "Sum.h"
#include "generic.hpp"

using namespace omnn::math;
using namespace boost::unit_test_framework;

std::string l(const omnn::math::Valuable& v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

BOOST_AUTO_TEST_CASE(Fraction_cmp_tests) {
    Valuable::OptimizeOff off;
    auto equal = 1_v / 2 == 2_v / 4_v;
    BOOST_TEST(equal);
}

BOOST_AUTO_TEST_CASE(Fraction_ordering_tests) {
    auto _1 = "((-16/25)*(1r5) + (16/5))^(1/2)"_v;
    auto _2 = "(3/5)*sqrt(5)"_v;
    InequalOrderCheck(_1, _2);
    {
        Valuable::OptimizeOff off;
        _1 = "((sqrt((-4*(caterpillar^2) + 40*caterpillar + -100)))/4)"_v;
        _2 = Fraction{26, 4};
        InequalOrderCheck(_1, _2);
    }
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
    
    BOOST_TEST(((2040_v*v1/(-2_v*v1))==-1020));
    
    _ = (2040_v/v1) / ((-1_v/v1)*v2);
    _.optimize();
    BOOST_TEST((_ == -2040_v/v2));
    
    auto f = Fraction(1,-2);
    BOOST_TEST((f < 0));

    _ = constants::plus_minus_1;
    auto eq = _ == constants::plus_minus_1;
    BOOST_TEST(eq);
    BOOST_TEST((_.IsMultival() == YesNoMaybe::Yes));
    _ /= 1_v^(1_v/2);
    BOOST_TEST((_.IsMultival() == YesNoMaybe::Yes));
    eq = _ == (1_v^(1_v/2));
    BOOST_TEST(eq);

    auto _1 = Valuable(0.000144);
    auto _2 = 144_v / 1000000;
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(IrrationalFraction_tests) {
    Valuable f = Fraction{constants::one, constants::i};
    f.optimize();
    BOOST_TEST(f == -constants::i);
}

BOOST_AUTO_TEST_CASE(FractionWithRadicalsSimplification_tests)
{
    auto sqrt2 = constants::two.Sqrt();
    auto _1 = -8 / (8 * sqrt2) + 1;
    auto _2 = 1 - 1/sqrt2;
    BOOST_TEST(static_cast<double>(_1) == static_cast<double>(_2));
}

BOOST_AUTO_TEST_CASE(FractionSimplification_tests){

	auto _1 = 25_v ^ Fraction{constants::minus_1,constants::two};
    auto _2 = constants::plus_minus_1 / 5;
	BOOST_TEST(_1 == _2);
    _2 = (1_v / 5)*(1_v^(1_v/2));
    BOOST_TEST(_1 == _2);

	DECL_VA(x);
	_1 =(-1_v)^x;
	_2 = (-1_v)^((1_v/2)*x + _1/4 + ((-1_v)/4));
    BOOST_TEST(_1 != _2);

    _1 = (x^2) / (x^1);
    _2 = x;
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(Fraction_optimization_no_hang_test
    , *timeout(2)
) {
    DECL_VARS(x);
    auto _ = constants::one / (2 * x);
    _.optimize();

    // FIXME : _ = Valuable("((x^2)-4)/((x^2)+2*x-6)");
    _.optimize();
}

BOOST_AUTO_TEST_CASE(Balancing_no_hang_test
    , *timeout(2)
) {
    DECL_VARS(x, y, z);
    auto _ = (constants::minus_1 / 4) * ((-16 * (y ^ 2) + 160 * y - 8 * x - 200) ^ constants::half) + z - 35;
    _.as<Sum>().balance();
}

BOOST_AUTO_TEST_CASE(Fraction_InCommonWith_test)
{
    // Test fraction with fraction
    auto f1 = Fraction(4, 6);  // 2/3
    auto f2 = Fraction(10, 15); // 2/3
    auto common = f1.InCommonWith(f2);
    BOOST_TEST((common == f1));
    // FIXME: common = f1.GCD(f2); 

    // Test fraction with integer
    f1 = Fraction(15, 5); // 3
    auto i = 9_v;
    common = f1.InCommonWith(i);
    BOOST_TEST((common == 3_v));
    // FIXME: common = f1.GCD(f2);

    // Test with no common factors
    f1 = Fraction(2, 3);
    f2 = Fraction(5, 7);
    common = f1.InCommonWith(f2);
    BOOST_TEST((common == constants::one));
    // FIXME: common = f1.GCD(f2);

    // Test with variable
    DECL_VA(x);
    f1 = Fraction(x, 2);
    f2 = Fraction(x, 4);
    common = f1.InCommonWith(f2);
    auto expected = x / 2;
    BOOST_TEST((common == expected));
    // FIXME: common = f1.GCD(f2);
}
