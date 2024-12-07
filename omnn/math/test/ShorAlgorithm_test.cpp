#define BOOST_TEST_MODULE ShorAlgorithm test
#include <boost/test/unit_test.hpp>
#include "../ShorAlgorithm.h"
#include "../QuantumRegister.h"
#include "../QuantumFourierTransform.h"
#include "../ModularExponentiation.h"
#include "../PeriodFinder.h"

using namespace omnn::math;

// Known factorization tests
BOOST_AUTO_TEST_CASE(ShorAlgorithm_small_composite_test) {
    // Test factoring 15 (3 * 5)
    auto result = ShorAlgorithm::factor(Integer(15));
    BOOST_REQUIRE(result.has_value());

    auto [factor1, factor2] = *result;
    BOOST_TEST((factor1 == 3 && factor2 == 5) || (factor1 == 5 && factor2 == 3));
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_medium_composite_test) {
    // Test factoring 91 (7 * 13)
    auto result = ShorAlgorithm::factor(Integer(91));
    BOOST_REQUIRE(result.has_value());

    auto [factor1, factor2] = *result;
    BOOST_TEST((factor1 == 7 && factor2 == 13) || (factor1 == 13 && factor2 == 7));
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_larger_composite_test) {
    // Test factoring 143 (11 * 13)
    auto result = ShorAlgorithm::factor(Integer(143));
    BOOST_REQUIRE(result.has_value());

    auto [factor1, factor2] = *result;
    BOOST_TEST((factor1 == 11 && factor2 == 13) || (factor1 == 13 && factor2 == 11));
}

// Edge cases and error handling
BOOST_AUTO_TEST_CASE(ShorAlgorithm_prime_input_test) {
    // Test with various prime numbers
    std::vector<Integer> primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
    for (const auto& prime : primes) {
        BOOST_CHECK_THROW(
            ShorAlgorithm::factor(prime),
            std::invalid_argument
        );
    }
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_invalid_input_test) {
    // Test with invalid inputs
    std::vector<Integer> invalid_inputs = {0, 1};
    for (const auto& n : invalid_inputs) {
        BOOST_CHECK_THROW(
            ShorAlgorithm::factor(n),
            std::invalid_argument
        );
    }
}

BOOST_AUTO_TEST_CASE(ShorAlgorithm_perfect_power_test) {
    // Test with perfect powers
    std::vector<Integer> perfect_powers = {4, 8, 9, 16, 25, 27, 32, 36, 49, 64};
    for (const auto& n : perfect_powers) {
        BOOST_CHECK_THROW(
            ShorAlgorithm::factor(n),
            std::invalid_argument
        );
    }
}

// Integration tests
BOOST_AUTO_TEST_CASE(ShorAlgorithm_quantum_circuit_integration_test) {
    // Test complete quantum pipeline with small number
    Integer N(15);
    Integer a(7);  // Known to work well with N=15

    // Create quantum register
    size_t precision_qubits = 8;
    size_t n = N.getBitLength();
    QuantumRegister reg(precision_qubits + 2 * n);

    // Initialize input register to superposition
    for (size_t i = 0; i < precision_qubits; ++i) {
        reg.hadamard(i);
    }

    // Apply modular exponentiation
    ModularExponentiation::apply(reg, a, N, 0, precision_qubits,
                               precision_qubits, precision_qubits + n);

    // Apply inverse QFT
    QuantumFourierTransform::inverse(reg, 0, precision_qubits);

    // Verify quantum state is valid
    const auto& state = reg.get_state();
    BOOST_TEST(state.size1() == (1ULL << (precision_qubits + 2 * n)));
}

// Performance tests
BOOST_AUTO_TEST_CASE(ShorAlgorithm_multiple_attempts_test) {
    // Test that algorithm eventually succeeds with multiple attempts
    Integer N(77);  // 7 * 11
    bool success = false;

    for (int attempt = 0; attempt < 5; ++attempt) {
        auto result = ShorAlgorithm::factor(N);
        if (result) {
            success = true;
            auto [factor1, factor2] = *result;
            BOOST_TEST((factor1 == 7 && factor2 == 11) ||
                      (factor1 == 11 && factor2 == 7));
            break;
        }
    }

    BOOST_TEST(success);
}
