#include "ShorAlgorithm.h"
#include "PeriodFinder.h"
#include <random>
#include <cmath>

namespace omnn::math {

std::optional<std::pair<Integer, Integer>> ShorAlgorithm::factorize(const Integer& N) {
    // Check input validity
    if (N <= 1) {
        throw std::invalid_argument("Number must be greater than 1");
    }

    // Check if N is even
    if (N % 2 == 0) {
        return std::make_pair(Integer(2), N / 2);
    }

    // Check if N is prime
    if (isPrime(N)) {
        return std::nullopt;
    }

    // Check if N is a perfect power
    if (isPerfectPower(N)) {
        throw std::invalid_argument("Perfect powers are not supported");
    }

    // Check for small prime factors
    auto trivial_factors = checkTrivialFactors(N);
    if (trivial_factors) {
        return trivial_factors;
    }

    // Main Shor's algorithm loop
    std::random_device rd;
    std::mt19937_64 gen(rd());

    // Convert N to size_t for distribution
    auto N_val = N.to_size_t();
    std::uniform_int_distribution<uint64_t> dist(2, N_val - 1);

    for (int attempt = 0; attempt < 20; ++attempt) {
        // Choose random base a
        Integer a(dist(gen));

        // Ensure a is coprime to N
        Integer g = gcd(a, N);
        if (g > 1) {
            return std::make_pair(g, N / g);
        }

        // Find period using quantum subroutine
        auto period = PeriodFinder::findPeriod(a, N);
        if (!period) {
            continue;
        }

        // Try to find factors using the period
        auto factors = findFactorsFromPeriod(N, a, *period);
        if (factors) {
            return factors;
        }
    }

    return std::nullopt;
}

bool ShorAlgorithm::isPrime(const Integer& n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    // Trial division up to sqrt(n)
    // Use explicit operator< to avoid ambiguity
    Integer i = 5;
    while (!operator<(n/i, i)) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
        i += 6;
    }
    return true;
}

bool ShorAlgorithm::isPerfectPower(const Integer& n) {
    // Check if n is a perfect power by trying all possible bases and exponents
    // For a number n, the maximum possible exponent is log2(n)
    Integer max_exp = 2;
    Integer temp = n;
    while (temp > 1) {
        temp = temp / 2;
        max_exp += 1;
    }

    // Try each possible exponent from 2 up to max_exp
    for (Integer b = 2; b <= max_exp; ++b) {
        // Binary search for the base
        Integer left = 1;
        Integer right = n;

        while (left <= right) {
            Integer mid = (left + right) / 2;
            Integer power = 1;
            bool overflow = false;

            // Calculate mid^b using safer multiplication
            for (Integer i = 0; i < b && !overflow; ++i) {
                // Check if multiplying by mid would exceed n
                if (n / mid < power) {
                    overflow = true;
                } else {
                    power *= mid;
                }
            }

            if (overflow) {
                right = mid - 1;
            } else if (power == n) {
                return true;
            } else if (power < n) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
    }
    return false;
}

Integer ShorAlgorithm::gcd(const Integer& a, const Integer& b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

std::optional<std::pair<Integer, Integer>> ShorAlgorithm::checkTrivialFactors(
    const Integer& N) {
    // Check small prime factors up to 100
    for (Integer i = 3; i <= 100; i += 2) {
        if (N % i == 0) {
            return std::make_pair(i, N / i);
        }
    }
    return std::nullopt;
}

std::optional<std::pair<Integer, Integer>> ShorAlgorithm::findFactorsFromPeriod(
    const Integer& N, const Integer& a, const Integer& r) {
    // Period must be even for finding factors
    if (r % 2 != 0) {
        return std::nullopt;
    }

    // Calculate a^(r/2) mod N
    Integer power = a.Power(r/2);
    power = power % N;

    // Check if we found a non-trivial factor
    if (power == N-1) {
        return std::nullopt;
    }

    Integer factor = gcd(power + 1, N);
    if (factor == 1 || factor == N) {
        factor = gcd(power - 1, N);
    }

    if (factor == 1 || factor == N) {
        return std::nullopt;
    }

    return std::make_pair(factor, N / factor);
}

} // namespace omnn::math
