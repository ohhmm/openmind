#include "../Variable.h"
#include "Logarithm.h"
#include <cassert>
#include <iostream>

using namespace omnn::math;

int main() {
    // Test the optimize method for a logarithm where the target is a power of the base
    {
        Valuable base = 2;
        Valuable target = base ^ 3; // 2^3
        Logarithm log(base, target);
        log.optimize();
        assert(log == 3 && "Logarithm optimization of base ^ target failed.");
    }

    // Test the optimize method for a logarithm where the base and target are the same
    {
        Valuable base = 10;
        Valuable target = 10;
        Logarithm log(base, target);
        log.optimize();
        assert(log == 1 && "Logarithm optimization of base == target failed.");
    }

    // Add more test cases as needed

    std::cout << "All Logarithm optimize tests passed." << std::endl;
    return 0;
}
