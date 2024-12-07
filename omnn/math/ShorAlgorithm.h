#pragma once

#include "Integer.h"
#include "PeriodFinder.h"
#include <utility>
#include <optional>

namespace omnn::math {

class ShorAlgorithm {
public:
    /**
     * Factors a composite number N into its prime factors using Shor's algorithm
     * @param N Number to factor (must be composite)
     * @return Pair of factors if factorization succeeds, std::nullopt otherwise
     */
    static std::optional<std::pair<Integer, Integer>> factor(const Integer& N);

private:
    // Helper function to check if number is suitable for factorization
    static bool validateInput(const Integer& N);

    // Helper function to choose random coprime number
    static Integer chooseRandomBase(const Integer& N);

    // Helper function to calculate GCD
    static Integer gcd(const Integer& a, const Integer& b);

    // Helper function to calculate factors from period
    static std::optional<std::pair<Integer, Integer>> calculateFactors(
        const Integer& N, const Integer& a, const Integer& period);
};

} // namespace omnn::math
