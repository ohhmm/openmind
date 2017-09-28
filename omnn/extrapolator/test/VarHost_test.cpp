/*
 * VarHost_test.cpp
 *
 *  Created on: 25 авг. 2017 г.
 *      Author: sergejkrivonos
 */

#include "VarHost.h"

#define BOOST_TEST_MODULE Varhost test
#include <boost/test/unit_test.hpp>
#include "SymmetricDouble.h"
#include "Polyfit.h"
#include "Variable.h"

using namespace std;
using namespace boost::unit_test;
using namespace omnn::extrapolator;


BOOST_AUTO_TEST_CASE(Varhost_test)
{
    TypedVarHost<int> vh;
    Variable v(vh);
    Variable v1(vh);
    BOOST_TEST(v!=v1);
    
    Variable a, b;
    BOOST_TEST(a!=b);
    BOOST_TEST(a==a);
    BOOST_TEST(b==b);
    
    BOOST_CHECK_THROW(a!=v, const char*);
}

