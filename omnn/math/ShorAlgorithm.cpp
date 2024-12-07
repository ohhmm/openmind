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
    std::uniform_int_distribution<uint64_t> dist(2, N.to_size_t() - 1);

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

    for (Integer i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

bool ShorAlgorithm::isPerfectPower(const Integer& n) {
    double ln_n = std::log(n.to_double());
    for (int b = 2; b <= std::log2(ln_n); ++b) {
        double a = std::exp(ln_n / b);
        Integer base(static_cast<uint64_t>(a));
        Integer power = 1;
        for (int i = 0; i < b; ++i) {
            power *= base;
            if (power > n) break;
        }
        if (power == n) return true;
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
    Integer power = 1;
    for (Integer i = 0; i < r/2; ++i) {
        power = (power * a) % N;
    }

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
