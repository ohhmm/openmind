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
            //std::cout << " evaluated(" << x << ',' << y << ") = " << copy << std::endl;
            BOOST_TEST(copy == etalon);

            copy = lambda(x, y);
            //std::cout << " lambda(" << x << ',' << y << ") = " << copy << std::endl;
            // FIXME: BOOST_TEST(copy == etalon);
        }
    }
}

void TestBooleanOperator(const Valuable& expressionXY, auto function) {
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
            copy.optimize();  // Add optimization step before IsZero check
            //std::cout << " evaluated(" << x << ',' << y << ") = " << copy << std::endl;
            BOOST_TEST(copy.IsZero() == etalon);

            copy = lambda(x, y);
            //std::cout << " lambda(" << x << ',' << y << ") = " << copy << std::endl;

            // FIXME: BOOST_TEST(copy.IsZero() == etalon);
            if(copy.IsZero() != etalon){
                std::cout << "Expected " << etalon << " lambda(" << x << ',' << y << ") = " << copy << std::endl;
            }
        }
    }
}

} // namespace

