// VarSwap_test.cpp
#define BOOST_TEST_MODULE VarSwap test
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>

// Include necessary headers
#include "Variable.h"
#include "VarHost.h"

using namespace std;
using namespace boost::unit_test;
using namespace omnn::math;

// Function to demonstrate x86 optimized variable swap
void optimized_swap(int& a, int& b) {
    // This function will be compiled to efficient x86 assembly
    // The classic XOR swap algorithm is often optimized by compilers
    a = a ^ b;
    b = a ^ b;
    a = a ^ b;
}

// Standard swap for comparison
void standard_swap(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

// Test to demonstrate the variable swap optimization
BOOST_AUTO_TEST_CASE(VarSwap_test)
{
    // Test with simple integers
    int x = 5, y = 10;
    
    // Store original values
    int original_x = x;
    int original_y = y;
    
    // Perform optimized swap
    optimized_swap(x, y);
    
    // Verify swap worked correctly
    BOOST_TEST(x == original_y);
    BOOST_TEST(y == original_x);
    
    // Reset values
    x = original_x;
    y = original_y;
    
    // Perform standard swap for comparison
    standard_swap(x, y);
    
    // Verify standard swap also works
    BOOST_TEST(x == original_y);
    BOOST_TEST(y == original_x);
    
    // Test with Variable objects from the codebase
    auto vh = VarHost::make<int>();
    auto var1 = vh->New();
    auto var2 = vh->New();
    
    // Verify variables are different
    BOOST_TEST(var1 != var2);
    
    // Store references to the variables
    Variable& v1 = var1;
    Variable& v2 = var2;
    
    // Store original values for verification
    Variable original_v1 = v1;
    Variable original_v2 = v2;
    
    // Swap using std::swap
    std::swap(v1, v2);
    
    // Verify swap worked correctly
    BOOST_TEST(v1 == original_v2);
    BOOST_TEST(v2 == original_v1);
}
