#define BOOST_TEST_MODULE PeriodFinder test
#include <boost/test/unit_test.hpp>
#include "../PeriodFinder.h"
#include <vector>
#include <utility>

using namespace omnn::math;

// Basic period finding tests
BOOST_AUTO_TEST_CASE(PeriodFinder_known_periods_test) {
    // Test cases with known periods
    std::vector<std::tuple<Integer, Integer, Integer>> test_cases = {
        {2, 15, 4},   // 2^4 mod 15 = 1
        {7, 15, 4},   // 7^4 mod 15 = 1
        {4, 21, 3},   // 4^3 mod 21 = 1
        {2, 33, 10},  // 2^10 mod 33 = 1
    };

    for (const auto& [base, modulus, expected_period] : test_cases) {
        auto period = PeriodFinder::findPeriod(base, modulus);
        BOOST_REQUIRE(period.has_value());
        BOOST_TEST(period.value() == expected_period);
    }
}

// Edge cases
BOOST_AUTO_TEST_CASE(PeriodFinder_error_handling_test) {
    // Test invalid inputs
    BOOST_CHECK_THROW(
        PeriodFinder::findPeriod(Integer(16), Integer(15)),
        std::invalid_argument
    );

    BOOST_CHECK_THROW(
        PeriodFinder::findPeriod(Integer(0), Integer(15)),
        std::invalid_argument
    );

    BOOST_CHECK_THROW(
        PeriodFinder::findPeriod(Integer(2), Integer(0)),
        std::invalid_argument
    );
}

// Prime modulus tests
BOOST_AUTO_TEST_CASE(PeriodFinder_prime_modulus_test) {
    std::vector<std::pair<Integer, Integer>> prime_test_cases = {
        {2, 7},   // Period should divide φ(7) = 6
        {3, 11},  // Period should divide φ(11) = 10
        {2, 13},  // Period should divide φ(13) = 12
        {5, 17},  // Period should divide φ(17) = 16
    };

    for (const auto& [base, prime] : prime_test_cases) {
        auto period = PeriodFinder::findPeriod(base, prime);
        BOOST_REQUIRE(period.has_value());

        // Period should divide φ(p) = p-1 for prime p
        BOOST_TEST((prime - 1) % period.value() == 0);
    }
}

// Performance tests
BOOST_AUTO_TEST_CASE(PeriodFinder_multiple_attempts_test) {
    // Test that period finding succeeds within reasonable attempts
    Integer base(3);
    Integer modulus(35);  // Period should be 12

    bool success = false;
    for (int attempt = 0; attempt < 5; ++attempt) {
        auto period = PeriodFinder::findPeriod(base, modulus);
        if (period && period.value() == 12) {
            success = true;
            break;
        }
    }

    BOOST_TEST(success);
}
