#define BOOST_TEST_MODULE Logic test
#include <boost/test/unit_test.hpp>

#include "Variable.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(ifz_tests)
{
    // two different bits
    // if a=0 then b=1 else b=0
    // if a=1 then b=0 else b=1
    Variable a,b;
    auto e = a.Equals(1).Ifz(b.Equals(0), b.Equals(1));
    {
        auto ee = e;
        ee.Eval(a, 0);
        ee.Eval(b, 1);
        ee.optimize();
        BOOST_TEST(ee==0);
    }
    {
        auto ee = e;
        ee.Eval(a, 1);
        ee.Eval(b, 0);
        ee.optimize();
        BOOST_TEST(ee==0);
    }
// TODO :   {
//        auto ee = e;
//        ee.Eval(a,1); // b == 0
//        ee.optimize();
//        auto solutions = ee(b);
//        BOOST_TEST(solutions.size()==1);
//        BOOST_TEST(*solutions.begin()==0);
//    }
// TODO :
//    {
//        auto ee = e;
//        ee.Eval(a, 0); // b == 1
//        ee.optimize();
//        auto solutions = ee(b);
//        BOOST_TEST(solutions.size()==1);
//        BOOST_TEST(*solutions.begin()==1);
//    }
}
