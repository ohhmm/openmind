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
    auto vh = VarHost::make<int>();
    auto v = vh->New();
    BOOST_TEST(v.getMaxVaExp() == 1);
    auto v1 = vh->New();
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

        Valuable tt(t.as<Variable>().Move());
        BOOST_TEST(tt==a);
    }
    
    {
        Valuable t(a);
        BOOST_TEST(t==a);
        
        t = a;
        BOOST_TEST(t==a);
        
        Valuable tt(t.as<Variable>().Move());
        BOOST_TEST(tt==a);
    }
    
    BOOST_CHECK_THROW(a!=v, const char*);
    
    {
        std::string s = "any str";
        boost::any a = s;
        auto _ = a;
        auto host = VarHost::make<std::string>();
        v1 = host->New(s);
        auto v2 = host->New(s);
        BOOST_TEST(v1==v2);
        auto host2 = VarHost::make<std::string>();
        v1 = host2->New(s);
        BOOST_CHECK_THROW((void)(v1!=v2), const char*);
        v1 = host->New(std::string("t"));
        BOOST_TEST(v1!=v2);
    }
    
    
}

