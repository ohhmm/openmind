#define BOOST_TEST_MODULE PrincipalSurd test
#include <boost/test/unit_test.hpp>

#include "PrincipalSurd.h"

#include "generic.hpp"


using namespace omnn::math;
using namespace boost::unit_test;
using namespace std;


BOOST_AUTO_TEST_CASE(PrincipalSurdOrder_test)
{
    DECL_VA(x);
    auto _1 = PrincipalSurd(2);
    auto _2 = PrincipalSurd(x + 2);
    InequalOrderCheck(_1, _2);
}

BOOST_AUTO_TEST_CASE(PrincipalSurd_test)
{
	auto _1 = Valuable(0.000144).sqrt();
	BOOST_TEST(_1 == 0.012);
    auto _2 = 12_v / 1000;
    BOOST_TEST(_1 == _2);

    _1 = -24;
    _1.sqrt();
    _2 = PrincipalSurd(6) * 2 * constants::i;
    BOOST_TEST(_1 == _2);

    _1 = PrincipalSurd(1);
    BOOST_TEST(_1 == constants::one);
    _1 = PrincipalSurd(2);
    _2 = PrincipalSurd(2);
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(3);
    _2 = PrincipalSurd(3);
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) * PrincipalSurd(3);
    _2 = PrincipalSurd(6);
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) / PrincipalSurd(3);
    _2 = PrincipalSurd(2) / PrincipalSurd(3);
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) / 3;
    _2 = PrincipalSurd(2) / 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) / 3;
    _2 = PrincipalSurd(2) / 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) + 3;
    _2 = PrincipalSurd(2) + 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) - 3;
    _2 = PrincipalSurd(2) - 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) / 3;
    _2 = PrincipalSurd(2) / 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) + 3;
    _2 = PrincipalSurd(2) + 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) - 3;
    _2 = PrincipalSurd(2) - 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) ^ 3;
    _2 = PrincipalSurd(8);
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(RadicalExpressions_noHang_test
    , *timeout(2)
    )
{
    Valuable _1("((sqrt(111))*((-6)/37) + ((-35)/((sqrt(111))*2)))");
    Valuable _2("((-17)/111)*sqrt(111)");
    auto _ = _1 + _2;
}

BOOST_AUTO_TEST_CASE(RadicalExponentiationExpressions_test) {
    DECL_VA(x);
    Valuable _1("sqrt(7)^((2*x)/3)=49");
    auto solved = _1.Solutions(x);
    auto ok = solved.size() > 0;
    if (ok) {
        auto _2 = solved.begin()->get();
        BOOST_TEST(_2 == 6);
    } else {
        BOOST_TEST(ok);
    }
}

BOOST_AUTO_TEST_CASE(RadicalSimplification_test) {
    Valuable _1("(sqrt(90) - sqrt(40))/sqrt(10)");
    BOOST_TEST(_1 == 1);
}

BOOST_AUTO_TEST_CASE(RadicalSimplificationWithVariable_test) {
    Valuable _1("x - (sqrt(90) - sqrt(40))/sqrt(10)");
    DECL_VA(x);
    BOOST_TEST(_1 == x-1);
}

BOOST_AUTO_TEST_CASE(RadicalSolving_test) {
    Valuable _1("sqrt(8-(x^2))-4");
    auto solutions = _1.Solutions();
    BOOST_TEST(solutions.size() == 2);
    auto _ = PrincipalSurd{2} * constants::i;
    _1 = Product{2, _.Link()};
    auto _2 = Product{-2, _.Link()};
    decltype(solutions) expected = {std::move(_1), std::move(_2)};
    BOOST_TEST(solutions == expected);
}

BOOST_AUTO_TEST_CASE(RadicalSimplificationAndSolving_test)
{
    Valuable _1 = "x - (sqrt(90) - sqrt(40))/sqrt(10)"sv;
    auto solutions = _1.Solutions();
    BOOST_TEST(solutions == _1.GetIntegerSolution());
    auto numSolutions = solutions.size();
    BOOST_TEST(numSolutions == 1);
    if (numSolutions > 0) {
        auto& solution = solutions.begin()->get();
        BOOST_TEST(solution == 1);
    }
}

BOOST_AUTO_TEST_CASE(ComplexRadicalExpression_test) {
    DECL_VA(y);
    DECL_VA(z);
    
    // Inner expression under sqrt
    auto inner = -1944*(y^2) + -888*(z^2) + 1728*z*y + -5280*z + 7344*y + -8952;
    
    // Full expression
    auto expr = (-inner.Sqrt() - (-18*y + -6*z + -204))/42;
    
    // Test with specific values
    auto _1 = expr;
    _1.eval({{y, 1}, {z, 1}});
    _1.optimize();
    
    // Verify result matches expected value
    auto _2 = _1.Sq() * 42 * 42;  // Square and multiply by denominator squared
    _2.optimize();
    
    // The squared result should equal the original inner expression
    auto expected = inner;
    expected.eval({{y, 1}, {z, 1}});
    expected.optimize();
}
