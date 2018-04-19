//
// Created by Sergej Krivonos on 4/19/18.
//
#define BOOST_TEST_MODULE Sh test
#include <boost/test/unit_test.hpp>

#include "Variable.h"

using namespace omnn::math;
using namespace boost::unit_test;
using namespace std;

BOOST_AUTO_TEST_CASE(And_test)
{
    Variable v;
    auto _1 = v+10;
    auto _2 = v+11;
    auto _ = _1.And(5, _2);
    auto t = _;
    t.Eval(v,0);
    t.optimize();
    BOOST_TEST(t == 10);
    t = _;
    t.Eval(v,1);
    t.optimize();
    BOOST_TEST(t == 8);
}

BOOST_AUTO_TEST_CASE(Or_test)
{
    Variable v;
    auto _1 = v+10;
    auto _2 = v+11;
    auto _ = _1.And(5, _2);
    auto t = _;
    t.Eval(v,0);
    t.optimize();
    BOOST_TEST(t == 11);
    t = _;
    t.Eval(v,1);
    t.optimize();
    BOOST_TEST(t == 15);
}
