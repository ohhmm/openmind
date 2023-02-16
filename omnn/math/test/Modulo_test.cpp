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

BOOST_AUTO_TEST_CASE(Modulo_IntOperatorLess_test)
{
    DECL_VA(X);
    DECL_VA(Y);
    auto L = X.IntModIsLessOp(Y);
    for (auto x = 10; x-- > 1;)
        for (auto y = 10; y-- > 1;) {
            auto less = x < y;
            auto l = L;
            l.eval({{X, x}, {Y, y}});
            auto ok = l.IsInt() && l == less;
            if (!ok) {
                BOOST_TEST(ok);
                std::cout << "X=" << x << " Y=" << y << ' ' << ok << ' ' << l << std::endl;
            }
        }
}
