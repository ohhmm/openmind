#include <iostream>

// Forward declaration of a function from the shared library
extern void generateCudaKernel();

int main() {
    std::cout << "Testing the CUDA kernel generator shared library...\n";

    // Call the function from the shared library
    generateCudaKernel();

    std::cout << "Test completed.\n";
    return 0;
}
