/*
 * Modulo_test.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: sergejkrivonos
 */
#define BOOST_TEST_MODULE Sum test
#include <boost/test/unit_test.hpp>
#include <omnn/math/Modulo.h>
#include "Sum.h"
#include "Variable.h"


using namespace omnn::math;
using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(Modulo
                     ,*disabled()
                     )
{
    Variable va;
    auto _ = (va + 5) / (va + 1);
    auto p = (va + 5) % (va + 1);
    BOOST_TEST(p == (4_v % (va + 1)));
    p.Eval(va, 2);
    p.optimize();
    BOOST_TEST(7_v % 3 == p);
}
