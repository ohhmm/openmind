/*
 * VarHost_test.cpp
 *
 *  Created on: 25 авг. 2017 г.
 *      Author: sergejkrivonos
 */
#define BOOST_TEST_MODULE Varhost test
#include "VarHost.h"

#include <boost/test/unit_test.hpp>
#include "SymmetricDouble.h"
#include "Polyfit.h"
#include "Variable.h"

using namespace std;
using namespace boost::unit_test;
using namespace omnn::math;


BOOST_AUTO_TEST_CASE(Varhost_test)
{
    TypedVarHost<int> vh;
    Variable v(vh);
    BOOST_TEST(v.getMaxVaExp() == 1);
    Variable v1(vh);
    BOOST_TEST(v!=v1);
    
    Variable a, b;
    BOOST_TEST(a!=b);
    BOOST_TEST(a==a);
    BOOST_TEST(b==b);
    BOOST_TEST(a.getCommonVars()!=b.getCommonVars());
    
    {
    Variable t(a);
    BOOST_TEST(t==a);
    t = a;
    BOOST_TEST(t==a);
    }
    
    {
        Valuable t(a.Clone());
        BOOST_TEST(t==a);
        
        t = a;
        BOOST_TEST(t==a);

        Valuable tt(const_cast<Variable*>(Variable::cast(t))->Move());
        BOOST_TEST(tt==a);
    }
    
    {
        Valuable t(a);
        BOOST_TEST(t==a);
        
        t = a;
        BOOST_TEST(t==a);
        
        Valuable tt(const_cast<Variable*>(Variable::cast(t))->Move());
        BOOST_TEST(tt==a);
    }
    
    BOOST_CHECK_THROW(a!=v, const char*);
    
    {
    std::string s = "any str";
    boost::any a = s;
    auto _ = a;
    }
}

