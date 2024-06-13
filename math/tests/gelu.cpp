#include <iostream>
#include <cmath>
#include <cassert>

// GELU activation function
double gelu(double x) {
    return 0.5 * x * (1 + std::tanh(std::sqrt(2 / M_PI) * (x + 0.044715 * std::pow(x, 3))));
}

// Test function for GELU
void test_gelu() {
    // Test cases
    assert(std::abs(gelu(0) - 0.0) < 1e-6);
    assert(std::abs(gelu(1) - 0.841344) < 1e-6);
    assert(std::abs(gelu(-1) - (-0.158655)) < 1e-6);
    assert(std::abs(gelu(10) - 10.0) < 1e-6);
    assert(std::abs(gelu(-10) - 0.0) < 1e-6);

    std::cout << "All GELU tests passed!" << std::endl;
}

int main() {
    test_gelu();
    return 0;
}
