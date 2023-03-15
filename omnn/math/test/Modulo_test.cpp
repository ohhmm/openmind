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
    DECL_VA(XisLessThanY);
    auto d = Y - X;
    auto isZero = d / -d + 1;

    auto isLessEquation = (XisLessThanY.Equals(0) && (X - Y)) //  X == Y
                                                              //|| (XisLessThanY.Equals(1) && ())
        ;

    auto dModBy2 = d % 2;
    auto L = X.IntMod_Less(Y);
    // dModBy2;
    std::cout << "X<Y = " << L << std::endl;
    for (auto x = 10; x-- > 1;) {
        for (auto y = 10; y-- > 1;) {
            auto less = x < y;
            auto l = L;
            {
                Valuable::OptimizeOff oo;
                std::cout << '\n'
                          << x << '<' << y << " = "
                          << " 1 - ((" << x << " % " << y << ") - " << x << ") / -" << y << std::endl;
                l.eval({{X, x}, {Y, y}});
                std::cout << x << '<' << y << " = " << l;
            }
            //l.optimize();
            //std::cout << " = " << l << std::endl;
            //std::cout << "(x-y) = " << x - y << std::endl;

			auto po = Y.IntMod_IsPositive();
            po.Eval(Y, y-5);
            po.optimize();
            std::cout << '\n' << y - 5 << " is positive: " << po << " - bool: ";
            po = po.ToBool();
            po.optimize();
            std::cout << po << std::endl;

            //if (x - y)
            //    std::cout << "(y-x)/(x-y) = " << (y - x) / (x - y) << std::endl;
            //std::cout << "(y-x) = " << y - x << std::endl;
            //if (y - x)
            //    std::cout << "(y-x)/(x-y) = " << (x - y) / (y - x) << std::endl;

            //std::cout << "(x-y)%2 = " << (x - y) % 2 << std::endl;
            //std::cout << "(y-x)%2 = " << (y - x) % 2 << std::endl;

            //std::cout << "(x % (x - 1)) = " << (x % (x - 1)) << std::endl;
            //std::cout << "((x - 1) % x) = " << ((x - 1) % x) << std::endl;
            //std::cout << "(x % (x - 1)) * ((x - 1) % x) = " << (x % (x - 1)) * ((x - 1) % x) << std::endl;

            //std::cout << "x isn't 0 = " << Valuable(x).BoolIntModNotZero() << std::endl;
            //std::cout << "x < 0 = " << Valuable(x).IntMod_Negative() << std::endl;
            //std::cout << "x sign = " << Valuable(x).IntModSign() << std::endl;
            //std::cout << "x < y = " << Valuable(x).IntModLess(y) << std::endl;

            auto ok = l.IsInt() && l == less;
            if (!ok) {
                std::cout << "X=" << x << " Y=" << y << ' ' << ok << ' ' << l << std::endl;
                BOOST_TEST(ok);
            }
        }
    }
}
