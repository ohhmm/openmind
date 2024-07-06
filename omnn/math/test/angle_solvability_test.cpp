#include <iostream>
#include <cmath>
#include <cassert>

// Function to calculate the missing angle in a triangle given the other two angles
double calculate_missing_angle(double angle1, double angle2) {
    return 180.0 - (angle1 + angle2);
}

void test_calculate_missing_angle() {
    // Test case 1: 60 degrees and 60 degrees
    double angle1 = 60.0;
    double angle2 = 60.0;
    double expected = 60.0;
    double result = calculate_missing_angle(angle1, angle2);
    assert(std::abs(result - expected) < 1e-9);

    // Test case 2: 45 degrees and 45 degrees
    angle1 = 45.0;
    angle2 = 45.0;
    expected = 90.0;
    result = calculate_missing_angle(angle1, angle2);
    assert(std::abs(result - expected) < 1e-9);

    // Test case 3: 30 degrees and 60 degrees
    angle1 = 30.0;
    angle2 = 60.0;
    expected = 90.0;
    result = calculate_missing_angle(angle1, angle2);
    assert(std::abs(result - expected) < 1e-9);

    // Test case 4: 90 degrees and 45 degrees
    angle1 = 90.0;
    angle2 = 45.0;
    expected = 45.0;
    result = calculate_missing_angle(angle1, angle2);
    assert(std::abs(result - expected) < 1e-9);

    std::cout << "All tests passed!" << std::endl;
}

int main() {
    test_calculate_missing_angle();
    return 0;
}
