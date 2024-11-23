/*
 * Modulo_test.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: sergejkrivonos
 */
#define BOOST_TEST_MODULE Modulo test
#include <omnn/math/Modulo.h>
#include <omnn/math/Fraction.h>
#include "generic.hpp"


BOOST_AUTO_TEST_CASE(Modulo_test)
{
    Variable va;
    auto _1 = va + 5;
    auto _2 = va + 1;
    auto _ = _1 / _2;
    // TODO : implement
	//BOOST_TEST(_ * _2 == _1);
    auto p = _1 % _2;
    BOOST_TEST(p == (4_v % (va + 1)));
    p.Eval(va, 2);
    p.optimize();
    BOOST_TEST(7_v % 3 == p);
}

BOOST_AUTO_TEST_CASE(NegativeFractionModulo_test) {
    auto numerator = Fraction(-2165_v, 4_v);  // -2165/4
    auto denominator = -770_v;                // -770
    std::cout << "Numerator value: " << numerator << std::endl;
    std::cout << "Denominator value: " << denominator << std::endl;
    auto result = numerator % denominator;    // (-2165/4) % (-770)
    std::cout << "Result value: " << result << std::endl;
    auto expected = -541.25_v;
    std::cout << "Expected value: " << expected << std::endl;
    BOOST_TEST(result == expected);
}

BOOST_AUTO_TEST_CASE(ModIntLess_comparator_test) {
    auto Less = X.IntMod_Less(Y);
    std::cout << "X<Y : " << Less << std::endl;
    TestBooleanOperator(Less, [](auto x, auto y) { return x < y; });
}

// According to ChatGPT:
// sgn(x) = (x > 0) - (x < 0)
// (x > 0) = (x % (x - 1)) * ((x - 1) % x)
// wrong and division by zero was ignored, but its more insightful then misleading
// thanks to the ChatGPT creators
BOOST_AUTO_TEST_CASE(Modulo_IntOperatorLess_test) {
    auto d = Y - X;
    auto isZero = d / -d + 1;


    auto IntLessOperator = X.IntMod_Less(Y)//.ToBool()
        //(X - Y + 1)*(X - Y + 2)*((Y - X) %(-X + Y - 1) - 1)
				;
    std::cout << "X<Y = " << IntLessOperator << std::endl;
    for (auto x = 10; x-- > 1;) {
        for (auto y = 10; y-- > 1;) {
            auto isLess = x < y;
            auto lessOperatorInstantiation = IntLessOperator;
            {
                Valuable::OptimizeOff off;
                std::cout << '\n' << x << '<' << y << " = ";
                lessOperatorInstantiation.eval({{X, x}, {Y, y}});
                std::cout << " expression that must be equal to zero when true: " << lessOperatorInstantiation << std::endl;
            }
            lessOperatorInstantiation.optimize();
            std::cout << std::endl << "Is " << x << '<' << y << " : " << lessOperatorInstantiation << std::endl;

			auto boolLessOp = lessOperatorInstantiation.ToBool();
            BOOST_TEST(boolLessOp.IsInt());
            BOOST_TEST(lessOperatorInstantiation.IsInt());
            auto b = lessOperatorInstantiation.IsInt() && lessOperatorInstantiation.ca() == 0;
            std::cout << std::endl << x << '<' << y << " = " << b << std::endl;
            BOOST_TEST(boolLessOp == b);

            auto ok = lessOperatorInstantiation.IsInt() && b == isLess;
            if (!ok) {
                std::cout << "X=" << x << " Y=" << y << ' ' << ok << " bool: " << b << std::endl;
                BOOST_TEST(ok);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(Modulo_Ordering_test) {
    auto _1 = "(1 % ((Y ^ 4)))"_v;
    auto _2 = "((-4 * (Y ^ 3)) % ((Y ^ 4)))"_v;
    InequalOrderCheck(_1, _2);
}
