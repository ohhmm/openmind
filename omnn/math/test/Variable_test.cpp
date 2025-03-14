//
// Created by Devin AI on 14.03.25.
//
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
    
    // Clone the variable
    Valuable cloned(a.Clone());
    
    // Test that the clone equals the original
    BOOST_TEST(cloned == a);
    
    // Test that they are different objects (not the same memory address)
    BOOST_TEST(&cloned.get() != &a);
    
    // Test with a variable from a specific VarHost
    auto vh = VarHost::make<int>();
    auto v = vh->New();
    Valuable clonedV(v.Clone());
    BOOST_TEST(clonedV == v);
    BOOST_TEST(&clonedV.get() != &v);
    
    // Test with a named variable
    Variable named("test_var");
    Valuable clonedNamed(named.Clone());
    BOOST_TEST(clonedNamed == named);
    BOOST_TEST(&clonedNamed.get() != &named);
    
    // Test that the clone maintains the same VarHost
    BOOST_TEST(named.getVaHost() == clonedNamed.as<Variable>().getVaHost());
    
    // Test cloning a variable with a specific value
    Variable x("x");
    Valuable clonedX(x.Clone());
    BOOST_TEST(clonedX == x);
    
    // Test that modifications to the clone don't affect the original
    clonedX += 5;
    BOOST_TEST(clonedX != x);
    BOOST_TEST(x == Variable("x"));
}
