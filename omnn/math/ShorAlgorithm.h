#pragma once

#include "Variable.h"  // Include Variable.h before Integer.h
#include "Integer.h"
#include <optional>
#include <utility>

namespace omnn::math {

class ShorAlgorithm {
public:
    // Find non-trivial factors of N
    static std::optional<std::pair<Integer, Integer>> factorize(const Integer& N);

private:
    // Helper functions
    static bool isPrime(const Integer& n);
    static bool isPerfectPower(const Integer& n);
    static Integer gcd(const Integer& a, const Integer& b);
    static std::optional<std::pair<Integer, Integer>> checkTrivialFactors(const Integer& N);
    static std::optional<std::pair<Integer, Integer>> findFactorsFromPeriod(
        const Integer& N, const Integer& a, const Integer& r);
};

} // namespace omnn::math
