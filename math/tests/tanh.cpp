#include <iostream>
#include <cmath>
#include <cassert>

// Tanh activation function
double tanh_activation(double x) {
    return std::tanh(x);
}

// Test function for Tanh
void test_tanh() {
    // Test cases
    assert(std::abs(tanh_activation(0) - 0.0) < 1e-6);
    assert(std::abs(tanh_activation(1) - 0.761594) < 1e-6);
    assert(std::abs(tanh_activation(-1) - (-0.761594)) < 1e-6);
    assert(std::abs(tanh_activation(10) - 1.0) < 1e-6);
    assert(std::abs(tanh_activation(-10) - (-1.0)) < 1e-6);

    std::cout << "All Tanh tests passed!" << std::endl;
}

int main() {
    test_tanh();
    return 0;
}
