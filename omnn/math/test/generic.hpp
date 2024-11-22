#pragma once
#include <boost/test/unit_test.hpp>

#include <omnn/math/Variable.h>

#include <cmath>

using namespace omnn::math;
using namespace boost::unit_test;


namespace {

DECL_VARS(X, Y);

void TestBinaryOperator(const Valuable& expressionXY, auto function) {
    //std::cout << " Cheking " << expressionXY << std::endl;
    auto lambda = expressionXY.CompiLambda(X, Y);
    for (auto x = 10; x --> -10 ;)
    {
        for (auto y = 10; y --> -10 ;)
        {
            auto etalon = function(x, y);
            //std::cout << " etalon(" << x << ',' << y << ") = " << etalon << std::endl;

            auto copy = expressionXY;
            Valuable::vars_cont_t evalMap = {{X, x}, {Y, y}};
            copy.eval(evalMap);
            copy.optimize();
            //std::cout << " evaluated(" << x << ',' << y << ") = " << copy << std::endl;
            BOOST_TEST(copy == etalon);

            copy = lambda(x, y);
            //std::cout << " lambda(" << x << ',' << y << ") = " << copy << std::endl;
            // FIXME: BOOST_TEST(copy == etalon);
            if(copy != etalon){
                std::cout << "Expected " << etalon << " lambda(" << x << ',' << y << ") = " << copy << std::endl;
            }
        }
    }
}

void TestBooleanOperator(const Valuable& expressionXY, auto function, bool compilambda = {}) {
    //std::cout << " Cheking " << expressionXY << std::endl;
    auto lambda = expressionXY.CompiLambda(X, Y);
    for (auto x = 10; x --> -10 ;)
    {
        for (auto y = 10; y --> -10 ;)
        {
            auto etalon = function(x, y);
            //std::cout << " etalon(" << x << ',' << y << ") = " << etalon << std::endl;

            auto copy = expressionXY;
            {
                Valuable::OptimizeOff off;  // Disable optimization during evaluation
                Valuable::vars_cont_t evalMap = {{X, x}, {Y, y}};
                copy.eval(evalMap);
                copy.optimize();
                bool result = copy.IsZero();
                BOOST_TEST(result == etalon,
                    "Eval path failed: x=" << x << ", y=" << y <<
                    ", expected=" << etalon << ", got=" << result);
            }

            // FIXME: should be always on
            if (compilambda) {
                copy = lambda(x, y);
                //std::cout << " lambda(" << x << ',' << y << ") = " << copy << std::endl;
                bool result = copy.IsZero();
                BOOST_TEST(result == etalon,
                    "Lambda path failed: x=" << x << ", y=" << y <<
                    ", expected=" << etalon << ", got=" << result);
            }
        }
    }
}

void TestXpression(const Valuable& expressionX, auto function, bool compilambda = {}) {
    auto lambda = expressionX.CompiLambda(X);
    for (auto x = 10; x --> -10 ;)
    {
        auto copy = expressionX;
        copy.Eval(X, x);
        copy.optimize();
        auto vars = copy.Vars();
        decltype(vars) requiredSolvingVars = {Y};
        if (vars == requiredSolvingVars) {
            Valuable::OptimizeOn on;
            copy = copy(Y).Optimized();
        }
        auto etalon = function(x);
        BOOST_TEST(copy == etalon, "f(X=" << x << ")=" << copy);

        copy = lambda(x);
        // FIXME: should be always on
        if (compilambda) {
            BOOST_TEST(copy == etalon);
        }
        if (copy != etalon) {
            BOOST_TEST_MESSAGE("Expected " << (etalon ? "true" : "false") << " lambda(" << x << ") = " << copy);
        }
    }
}

void TestBooleanExpression(const Valuable& expressionX, auto function) {
    auto lambda = expressionX.CompiLambda(X);
    for (auto x = 10; x-- > -10;) {
        auto etalon = function(x);
        auto copy = expressionX;
        {
            Valuable::OptimizeOff off;
            copy.eval({{X, x}});
        }
        copy.optimize();
        auto evaluatedToZero = copy.IsZero();
        BOOST_TEST(evaluatedToZero == etalon);

        copy = lambda(x);
        evaluatedToZero = copy.IsZero();
        // FIXME: BOOST_TEST(evaluatedToZero == etalon);
        if (evaluatedToZero != etalon) {
            BOOST_TEST_MESSAGE("Expected " << (etalon ? "true" : "false") << " lambda(" << x << ") = " << copy);
        }
    }
}

} // namespace

