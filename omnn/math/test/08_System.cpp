//
// Created by Sergej Krivonos on 25.02.18.
//
#define BOOST_TEST_MODULE System test
#include <boost/test/unit_test.hpp>

#include "System.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(System_tests)
{
    {
        System sys;
        Variable a,b;
        sys << a - 8 - b;
        sys << a + b - 100;
        auto s = sys.Solve(a);
        BOOST_TEST(s.size()==1);
        auto _ = *s.begin();
        BOOST_TEST(_ == 54);
    }
    
    {
        System s;
        Variable a,b;
        s << a - 8 - b;
        s << a + b - 21;
        auto _ = s.Solve(a);
        BOOST_TEST(_.size()==1);
    }
}
