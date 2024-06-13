#include <iostream>
#include <cmath>
#include <cassert>

// ReLU activation function
double relu(double x) {
    return std::max(0.0, x);
}

// Test function for ReLU
void test_relu() {
    // Test cases
    assert(relu(0) == 0);
    assert(relu(1) == 1);
    assert(relu(-1) == 0);
    assert(relu(10) == 10);
    assert(relu(-10) == 0);

    std::cout << "All ReLU tests passed!" << std::endl;
}

int main() {
    test_relu();
    return 0;
}
