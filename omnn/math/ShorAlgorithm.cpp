#include "ShorAlgorithm.h"
#include "PeriodFinder.h"
#include <random>
#include <cmath>

namespace omnn::math {

std::optional<std::pair<Integer, Integer>> ShorAlgorithm::factorize(const Integer& N) {
    if (N <= 1) {
        throw std::invalid_argument("Number must be greater than 1");
    }

    if (N % 2 == 0) {
        return std::make_pair(Integer(2), N / 2);
    }

    if (isPrime(N)) {
        return std::nullopt;
    }

    if (isPerfectPower(N)) {
        throw std::invalid_argument("Perfect powers are not supported");
    }

    auto trivial_factors = checkTrivialFactors(N);
    if (trivial_factors) {
        return trivial_factors;
    }

    std::random_device rd;
    std::mt19937_64 gen(rd());

    auto N_val = N.to_size_t();
    std::uniform_int_distribution<uint64_t> dist(2, N_val - 1);

    for (int attempt = 0; attempt < 20; ++attempt) {
        Integer a(dist(gen));

        Integer g = gcd(a, N);
        if (g > 1) {
            return std::make_pair(g, N / g);
        }

        auto period = PeriodFinder::findPeriod(a, N);
        if (!period) {
            continue;
        }

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

    for (Integer i = 5; i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

bool ShorAlgorithm::isPerfectPower(const Integer& n) {
    if (!(Integer(1) < n)) return false;

    Integer max_exp(2);
    Integer temp = n;
    while (Integer(1) < temp) {
        temp = temp / Integer(2);
        max_exp = max_exp + Integer(1);
    }

    for (Integer b(2); b < max_exp || !(max_exp < b); b = b + Integer(1)) {
        Integer left(1);
        Integer right = n;

        while (!(right < left)) {
            Integer mid = (left + right) / Integer(2);
            Integer power(1);
            bool overflow = false;

            for (Integer i(0); i < b && !overflow; i = i + Integer(1)) {
                Integer max_allowed = n / mid;
                if (power < max_allowed) {
                    power = power * mid;
                } else {
                    overflow = true;
                }
            }

            if (overflow || n < power) {
                right = mid - Integer(1);
            } else if (!(power < n) && !(n < power)) {
                return true;
            } else {
                left = mid + Integer(1);
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
    for (Integer i = 3; i <= 100; i += 2) {
        if (N % i == 0) {
            return std::make_pair(i, N / i);
        }
    }
    return std::nullopt;
}

std::optional<std::pair<Integer, Integer>> ShorAlgorithm::findFactorsFromPeriod(
    const Integer& N, const Integer& a, const Integer& r) {
    if (r % 2 != 0) {
        return std::nullopt;
    }

    Integer power = a.Power(r/2);
    power = power % N;

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
