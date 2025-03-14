/*
 * Variable_test.cpp
 *
 * Created on: 14 Mar 2025
 * Author: Devin AI
 */
#define BOOST_TEST_MODULE Variable test
#include <boost/test/unit_test.hpp>

#include "Variable.h"
#include "VarHost.h"

using namespace std;
using namespace boost::unit_test;
using namespace omnn::math;

BOOST_AUTO_TEST_CASE(Variable_Clone_test)
{
    // Create a Variable
    Variable a;
    
    // Test cloning through Valuable constructor (which uses Clone() internally)
    {
        Valuable t(a);
        BOOST_TEST(t == a);
        
        t = a;
        BOOST_TEST(t == a);
        
        Valuable tt(t.as<Variable>().Move());
        BOOST_TEST(tt == a);
    }
    
    // Test with a variable from a specific VarHost
    auto vh = VarHost::make<int>();
    auto v = vh->New();
    {
        Valuable t(v);
        BOOST_TEST(t == v);
        BOOST_TEST(&t.get() != &v);
    }
    
    // Test with a named variable
    Variable named("test_var");
    {
        Valuable t(named);
        BOOST_TEST(t == named);
        BOOST_TEST(&t.get() != &named);
        
        // Test that the clone maintains the same VarHost
        BOOST_TEST(named.getVaHost() == t.as<Variable>().getVaHost());
    }
    
    // Test that modifications to the clone don't affect the original
    {
        Variable x("x");
        Valuable clonedX = x;
        BOOST_TEST(clonedX == x);
        
        clonedX += 5;
        BOOST_TEST(clonedX != x);
        BOOST_TEST(x == Variable("x"));
    }
}
