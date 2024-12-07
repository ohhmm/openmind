#include "ShorAlgorithm.h"
#include <random>
#include <boost/random.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace omnn::math {

std::optional<std::pair<Integer, Integer>> ShorAlgorithm::factor(const Integer& N) {
    if (!validateInput(N)) {
        throw std::invalid_argument("Input must be composite number greater than 1");
    }

    // Maximum attempts to find factors
    const size_t max_attempts = 10;

    for (size_t attempt = 0; attempt < max_attempts; ++attempt) {
        // Choose random base a
        Integer a = chooseRandomBase(N);

        // Find period using quantum subroutine
        auto period = PeriodFinder::findPeriod(a, N);
        if (!period) {
            continue;
        }

        // Calculate factors using period
        auto factors = calculateFactors(N, a, *period);
        if (factors) {
            return factors;
        }
    }

    return std::nullopt;
}

bool ShorAlgorithm::validateInput(const Integer& N) {
    if (N <= 1) {
        return false;
    }

    // Check if N is prime
    if (N.IsPrime()) {
        return false;
    }

    // Check if N is even (handle separately for efficiency)
    if (N % 2 == 0) {
        return true;
    }

    // Check if N is a perfect power
    double log2_N = std::log2(N.AsDouble());
    for (int k = 2; k <= log2_N; ++k) {
        double root = std::pow(N.AsDouble(), 1.0/k);
        Integer possible_base(root);
        if (N == Integer::Power(possible_base, k)) {
            return false;
        }
    }

    return true;
}

Integer ShorAlgorithm::chooseRandomBase(const Integer& N) {
    // Use Boost random number generator for large integers
    static boost::random::mt19937 gen(std::random_device{}());
    boost::random::uniform_int_distribution<boost::multiprecision::cpp_int> dist(2, N-1);

    Integer a;
    do {
        a = Integer(dist(gen));
    } while (gcd(a, N) != 1);

    return a;
}

Integer ShorAlgorithm::gcd(const Integer& a, const Integer& b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

std::optional<std::pair<Integer, Integer>> ShorAlgorithm::calculateFactors(
    const Integer& N, const Integer& a, const Integer& period) {

    // Period must be even for factorization
    if (period % 2 != 0) {
        return std::nullopt;
    }

    // Calculate a^(r/2) mod N
    Integer power = Integer::Power(a, period/2) % N;

    // Check if power is -1 mod N
    if (power == N-1) {
        return std::nullopt;
    }

    // Calculate potential factors
    Integer factor1 = gcd(power + 1, N);
    Integer factor2 = gcd(power - 1, N);

    // Check if we found non-trivial factors
    if (factor1 == 1 || factor1 == N || factor2 == 1 || factor2 == N) {
        return std::nullopt;
    }

    return std::make_optional(std::make_pair(factor1, N/factor1));
}

} // namespace omnn::math
