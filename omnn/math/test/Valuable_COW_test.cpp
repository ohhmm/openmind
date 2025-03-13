#define BOOST_TEST_MODULE Valuable COW test
#include <boost/test/unit_test.hpp>

#include <omnn/math/Valuable.h>

using namespace omnn::math;

BOOST_AUTO_TEST_SUITE(ValuableCOWTestSuite)

// Test that Copy-on-Write works correctly when modifying a shared object
BOOST_AUTO_TEST_CASE(TestCopyOnWriteModification)
{
    // Create a Valuable object
    Valuable v1 = 42_v;
    
    // Create a second reference to the same object
    Valuable v2 = v1;
    
    // Verify they are equal before modification
    BOOST_CHECK_EQUAL(v1, v2);
    // FIXME : BOOST_CHECK(v1.getInst() == v2.getInst());
    // FIXME : BOOST_CHECK(v1.getInst().get() == v2.getInst().get());
    
    // Modify v2
    v2 += 10_v;
    
    // Verify that v1 is unchanged (COW should have created a copy for v2)
    BOOST_CHECK_EQUAL(v1, 42_v);
    BOOST_CHECK_EQUAL(v2, 52_v);
}

// Test that COW works with the get() method
BOOST_AUTO_TEST_CASE(TestCopyOnWriteWithGet)
{
    // Create a Valuable object
    Valuable v1 = 42_v;
    
    // Create a second reference to the same object
    Valuable v2 = v1;
    
    // Verify they are equal before modification
    BOOST_CHECK_EQUAL(v1, v2);
    
    // Modify v2 using get()
    Valuable& ref = v2.get();
    ref += 10_v;
    
    // Verify that v1 is unchanged (COW should have created a copy for v2)
    BOOST_CHECK_EQUAL(v1, 42_v);
    BOOST_CHECK_EQUAL(v2, 52_v);
}

// Test that COW works with various modifying methods
BOOST_AUTO_TEST_CASE(TestCopyOnWriteWithModifyingMethods)
{
    // Test with operator++
    {
        Valuable v1 = 42_v;
        Valuable v2 = v1;
        ++v2;
        BOOST_CHECK_EQUAL(v1, 42_v);
        BOOST_CHECK_EQUAL(v2, 43_v);
    }
    
    // Test with operator--
    {
        Valuable v1 = 42_v;
        Valuable v2 = v1;
        --v2;
        BOOST_CHECK_EQUAL(v1, 42_v);
        BOOST_CHECK_EQUAL(v2, 41_v);
    }
    
    // Test with operator*=
    {
        Valuable v1 = 42_v;
        Valuable v2 = v1;
        v2 *= 2_v;
        BOOST_CHECK_EQUAL(v1, 42_v);
        BOOST_CHECK_EQUAL(v2, 84_v);
    }
    
    // Test with sq()
    {
        Valuable v1 = 42_v;
        Valuable v2 = v1;
        v2.sq();
        BOOST_CHECK_EQUAL(v1, 42_v);
        BOOST_CHECK_EQUAL(v2, 42_v * 42_v);
    }
    
    // Test with sqrt()
    {
        Valuable v1 = 64_v;
        Valuable v2 = v1;
        v2.sqrt();
        BOOST_CHECK_EQUAL(v1, 64_v);
        BOOST_CHECK_EQUAL(v2, 8_v);
    }
    
    // Test with shl()
    {
        Valuable v1 = 42_v;
        Valuable v2 = v1;
        v2.shl();
        BOOST_CHECK_EQUAL(v1, 42_v);
        BOOST_CHECK_EQUAL(v2, 84_v);
    }
    
    // Test with shr()
    {
        Valuable v1 = 42_v;
        Valuable v2 = v1;
        v2.shr();
        BOOST_CHECK_EQUAL(v1, 42_v);
        BOOST_CHECK_EQUAL(v2, 21_v);
    }
}

// Test that COW works with call_polymorphic_method
BOOST_AUTO_TEST_CASE(TestCopyOnWriteWithCallPolymorphicMethod)
{
    // Create a Valuable object
    Valuable v1 = 42_v;
    
    // Create a second reference to the same object
    Valuable v2 = v1;
    
    // Verify they are equal before modification
    BOOST_CHECK_EQUAL(v1, v2);
    
    // Modify v2 using a method that calls call_polymorphic_method internally
    v2 += 10_v;
    
    // Verify that v1 is unchanged (COW should have created a copy for v2)
    BOOST_CHECK_EQUAL(v1, 42_v);
    BOOST_CHECK_EQUAL(v2, 52_v);
}

// Test that COW doesn't create unnecessary copies
BOOST_AUTO_TEST_CASE(TestCopyOnWriteEfficiency)
{
    // Create a Valuable object
    Valuable v1 = 42_v;
    
    // Create a second reference to the same object
    Valuable v2 = v1;
    
    // Create a third reference to the same object
    Valuable v3 = v1;
    
    // Modify v2
    v2 += 10_v;
    
    // Verify that v1 and v3 are unchanged and still equal
    BOOST_CHECK_EQUAL(v1, 42_v);
    BOOST_CHECK_EQUAL(v3, 42_v);
    BOOST_CHECK_EQUAL(v2, 52_v);
    
    // Modify v3
    v3 *= 2_v;
    
    // Verify that v1 is unchanged
    BOOST_CHECK_EQUAL(v1, 42_v);
    BOOST_CHECK_EQUAL(v2, 52_v);
    BOOST_CHECK_EQUAL(v3, 84_v);
}

BOOST_AUTO_TEST_CASE(TestCopyOnWriteMove)
{
    // Create a Valuable object
    Valuable v1 = 42_v;

    // Create a second reference to the same object
    Valuable v2 = v1;

    // Verify they are equal before modification
    BOOST_CHECK_EQUAL(v1, v2);
    // FIXME : BOOST_CHECK(v1.getInst() == v2.getInst());
    // FIXME : BOOST_CHECK(v1.getInst().get() == v2.getInst().get());

    // Modify v2
    auto moved = std::move(v2);

    // Verify that v1 is unchanged (COW should have created a copy for v2)
    BOOST_CHECK_EQUAL(v1, 42_v);
    BOOST_CHECK_EQUAL(moved, 42);
    BOOST_CHECK(nullptr == v2.getInst());
    // FIXME : BOOST_CHECK(v1.getInst() == moved.getInst());
    // FIXME : BOOST_CHECK(v1.getInst().get() == moved.getInst().get());
}

BOOST_AUTO_TEST_SUITE_END()
