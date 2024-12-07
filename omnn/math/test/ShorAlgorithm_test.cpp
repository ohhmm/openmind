#define BOOST_TEST_MODULE ShorAlgorithm test
#include <boost/test/unit_test.hpp>
#include "../ShorAlgorithm.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(ShorAlgorithm_small_composite_test) {
    // Test factoring 15 (3 * 5)
    auto result = ShorAlgorithm::factor(Integer(15));
    BOOST_REQUIRE(result.has_value());

    auto [factor1, factor2] = *result;
    BOOST_TEST((factor1 == 3 && factor2 == 5) || (factor1 == 5 && factor2 == 3));
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_prime_input_test) {
    // Test with prime number (should throw)
    BOOST_CHECK_THROW(
        ShorAlgorithm::factor(Integer(17)),
        std::invalid_argument
    );
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_invalid_input_test) {
    // Test with invalid inputs
    BOOST_CHECK_THROW(
        ShorAlgorithm::factor(Integer(1)),
        std::invalid_argument
    );

    BOOST_CHECK_THROW(
        ShorAlgorithm::factor(Integer(0)),
        std::invalid_argument
    );
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_power_of_prime_test) {
    // Test with perfect power (should throw)
    BOOST_CHECK_THROW(
        ShorAlgorithm::factor(Integer(25)),  // 5^2
        std::invalid_argument
    );
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_larger_composite_test) {
    // Test factoring 91 (7 * 13)
    auto result = ShorAlgorithm::factor(Integer(91));
    BOOST_REQUIRE(result.has_value());

    auto [factor1, factor2] = *result;
    BOOST_TEST((factor1 == 7 && factor2 == 13) || (factor1 == 13 && factor2 == 7));
}
