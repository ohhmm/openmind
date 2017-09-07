//
// Created by Сергей Кривонос on 02.09.17.
//
#define BOOST_TEST_MODULE Polyfit test
#include <boost/test/unit_test.hpp>

#include "Integer.h"

BOOST_AUTO_TEST_CASE(Integer_tests)
{
    omnn::extrapolator::Integer a = 2, b = 31;
    ++a;
    --a;
    a++;
    a--;
    
    //for(;b-->0;)
    {
        a *= a;
    }

    std::stringstream s;
    s << a;
    BOOST_TEST(s.str() == "4");
}
