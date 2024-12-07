#define BOOST_TEST_MODULE PeriodFinder test
#include <boost/test/unit_test.hpp>
#include "../PeriodFinder.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(PeriodFinder_small_numbers_test) {
    // Test with known period: 2^4 mod 15 = 1, period = 4
    auto period = PeriodFinder::findPeriod(Integer(2), Integer(15));
    BOOST_REQUIRE(period.has_value());
    BOOST_TEST(period.value() == 4);
}

BOOST_AUTO_TEST_CASE(PeriodFinder_coprime_test) {
    // Test with coprime numbers: 7^r mod 15
    auto period = PeriodFinder::findPeriod(Integer(7), Integer(15));
    BOOST_REQUIRE(period.has_value());
    BOOST_TEST(period.value() == 4);  // phi(15) = 8, period divides phi(15)
}

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
}

BOOST_AUTO_TEST_CASE(PeriodFinder_prime_modulus_test) {
    // Test with prime modulus: 2^r mod 7
    auto period = PeriodFinder::findPeriod(Integer(2), Integer(7));
    BOOST_REQUIRE(period.has_value());
    // Period should divide phi(7) = 6
    BOOST_TEST(6 % period.value() == 0);
}

BOOST_AUTO_TEST_CASE(PeriodFinder_power_of_two_test) {
    // Test with power of two base: 4^r mod 15
    auto period = PeriodFinder::findPeriod(Integer(4), Integer(15));
    BOOST_REQUIRE(period.has_value());
    BOOST_TEST(period.value() == 2);  // 4^2 mod 15 = 1
}
