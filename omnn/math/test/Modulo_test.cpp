/*
 * Modulo_test.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: sergejkrivonos
 */
#define BOOST_TEST_MODULE Modulo test
#include <boost/test/unit_test.hpp>
#include <omnn/math/Modulo.h>
#include <omnn/math/Variable.h>

using namespace omnn::math;
using namespace boost::unit_test;

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

// According to ChatGPT:
// sgn(x) = (x > 0) - (x < 0)
// (x > 0) = (x % (x - 1)) * ((x - 1) % x)
// wrong and division by zero was ignored, but its more insightful then missleading
// thanks to the ChatGPT creators
BOOST_AUTO_TEST_CASE(Modulo_IntOperatorLess_test) {
    DECL_VA(X);
    DECL_VA(Y);
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
                Valuable::OptimizeOff oo;
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
