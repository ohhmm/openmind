//
// Created by Сергей Кривонос on 02.09.17.
//
#define BOOST_TEST_MODULE Polyfit test
#include <boost/test/unit_test.hpp>

#include "Fraction.h"

BOOST_AUTO_TEST_CASE(Integer_tests)
{
    using f_t = omnn::extrapolator::Fraction;
    const f_t a (1, 2);
    f_t b (2,1);
    
    BOOST_TEST(a*b==f_t(1,1));
}
