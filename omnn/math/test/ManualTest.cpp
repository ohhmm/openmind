#include "CompilerCompatibility.h"
#include <iostream>
#include "Integer.h"
#include "Variable.h" // Include Variable.h to provide the complete definition of Variable

int main() {
    using namespace omnn::math;

    // Test cases
    Integer numbers[] = {2, 3, 4, 10, 17, 28, 29, 1};

    for (const auto& num : numbers) {
        std::cout << "Testing number: " << num << std::endl;
        auto firstFactor = num.FirstFactor();
        auto lastFactor = num.LastFactor();
        std::cout << "First factor of " << num << " is: " << firstFactor << std::endl;
        std::cout << "Last factor of " << num << " is: " << lastFactor << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
