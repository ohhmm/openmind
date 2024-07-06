#define BOOST_TEST_MODULE deduce_missing_angle test
#include <boost/test/unit_test.hpp>

#include "Variable.h"

// Function to calculate the missing angle in a triangle given the other two angles
auto calculate_missing_angle(auto angle1, auto angle2) {
    return 180.0 - (angle1 + angle2);
}

BOOST_AUTO_TEST_CASE(test_calculate_missing_angle)
{
    // Test case 1: 60 degrees and 60 degrees
    auto angle1 = 60_v;
    auto angle2 = angle1;
    auto expected = angle1;
    auto result = calculate_missing_angle(angle1, angle2);
    BOOST_TEST(result - expected == 0);

    // Test case 2: 45 degrees and 45 degrees
    angle1 = 45.0;
    angle2 = 45.0;
    expected = 90.0;
    result = calculate_missing_angle(angle1, angle2);
    BOOST_TEST(result - expected == 0);

    // Test case 3: 30 degrees and 60 degrees
    angle1 = 30.0;
    angle2 = 60.0;
    expected = 90.0;
    result = calculate_missing_angle(angle1, angle2);
    BOOST_TEST(result - expected == 0);

    // Test case 4: 90 degrees and 45 degrees
    angle1 = 90.0;
    angle2 = 45.0;
    expected = 45.0;
    result = calculate_missing_angle(angle1, angle2);
    BOOST_TEST(result - expected == 0);
}

