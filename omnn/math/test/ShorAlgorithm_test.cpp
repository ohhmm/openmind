#define BOOST_TEST_MODULE ShorAlgorithm test
#include <boost/test/unit_test.hpp>
#include "../ShorAlgorithm.h"
#include <vector>
#include <utility>

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(ShorAlgorithm_known_composites_test) {
    // Test cases with known factorizations
    std::vector<std::tuple<Integer, Integer, Integer>> test_cases = {
        {15, 3, 5},    // 3 * 5
        {91, 7, 13},   // 7 * 13
        {143, 11, 13}, // 11 * 13
    };

    for (const auto& [n, factor1, factor2] : test_cases) {
        auto result = ShorAlgorithm::factorize(n);
        BOOST_REQUIRE(result.has_value());
        auto [f1, f2] = result.value();

        bool matches_first_way = (f1 == factor1 && f2 == factor2);
        bool matches_second_way = (f1 == factor2 && f2 == factor1);
        BOOST_CHECK(matches_first_way || matches_second_way);
    }
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_prime_numbers_test) {
    std::vector<Integer> primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

    for (const auto& prime : primes) {
        auto result = ShorAlgorithm::factorize(prime);
        BOOST_CHECK(!result.has_value());
    }
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_error_handling_test) {
    // Test invalid inputs
    BOOST_CHECK_THROW(
        ShorAlgorithm::factorize(Integer(1)),
        std::invalid_argument
    );

    BOOST_CHECK_THROW(
        ShorAlgorithm::factorize(Integer(16)), // 2^4, perfect power
        std::invalid_argument
    );
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_quantum_pipeline_test) {
    // Test complete quantum pipeline with a simple case
    Integer N(15);
    auto result = ShorAlgorithm::factorize(N);

    BOOST_REQUIRE(result.has_value());
    auto [f1, f2] = result.value();
    BOOST_CHECK(f1 * f2 == N);

    bool matches_first_way = (f1 == 3 && f2 == 5);
    bool matches_second_way = (f1 == 5 && f2 == 3);
    BOOST_CHECK(matches_first_way || matches_second_way);
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_performance_test) {
    // Test that factorization succeeds within reasonable attempts
    Integer N(77); // 7 * 11

    bool success = false;
    for (int attempt = 0; attempt < 5; ++attempt) {
        auto result = ShorAlgorithm::factorize(N);
        if (result) {
            auto [f1, f2] = result.value();
            bool matches_first_way = (f1 == 7 && f2 == 11);
            bool matches_second_way = (f1 == 11 && f2 == 7);
            if (matches_first_way || matches_second_way) {
                success = true;
                break;
            }
        }
    }

    BOOST_CHECK(success);
}
