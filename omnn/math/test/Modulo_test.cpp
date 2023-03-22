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


    auto L = X.IntMod_Less(Y)
				//.ToBool()
				;
    std::cout << "X<Y = " << L << std::endl;
    for (auto x = 10; x-- > 1;) {
        for (auto y = 10; y-- > 1;) {
            auto less = x < y;
            auto l = L;
            {
                Valuable::OptimizeOff oo;
                std::cout << '\n' << x << '<' << y << " = ";
                l.eval({{X, x}, {Y, y}});
                std::cout << " expression that must be equal to zero when true: " << l << std::endl;
            }
            l.optimize();
            auto b = l.IsInt() && l.ca() == 0
					//.ToBool()
					;
            std::cout << std::endl << "Is " << x << '<' << y << " : " << l << std::endl;
            std::cout << std::endl << x << '<' << y << " = " << b << std::endl;
            


            auto ok = l.IsInt() && b == less;
            if (!ok) {
                std::cout << "X=" << x << " Y=" << y << ' ' << ok << " bool: " << b << std::endl;
                BOOST_TEST(ok);
            }
        }
    }
}
