#define BOOST_TEST_MODULE QuantumFourierTransform test
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <boost/test/unit_test.hpp>
#include "../QuantumFourierTransform.h"
#include <cmath>

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(QuantumFourierTransform_basic_test) {
    QuantumRegister reg(2);

    // Initialize to |01⟩
    reg.hadamard(0);
    reg.phase(0, M_PI);
    reg.hadamard(0);

    // Apply QFT
    QuantumFourierTransform::apply(reg, 0, 2);

    const auto& state = reg.get_state();
    double inv_sqrt2 = 1.0 / std::sqrt(2.0);

    // Expected state after QFT of |01⟩
    BOOST_TEST(std::abs(state[0].value().real() - inv_sqrt2) < 1e-10);
    BOOST_TEST(std::abs(state[1].value().imag() + inv_sqrt2) < 1e-10);
    BOOST_TEST(std::abs(state[2].value().real() + inv_sqrt2) < 1e-10);
    BOOST_TEST(std::abs(state[3].value().imag() - inv_sqrt2) < 1e-10);
}

BOOST_AUTO_TEST_CASE(QuantumFourierTransform_inverse_test) {
    QuantumRegister reg(2);

    // Initialize to |01⟩
    reg.hadamard(0);
    reg.phase(0, M_PI);
    reg.hadamard(0);

    // Apply QFT followed by inverse QFT
    QuantumFourierTransform::apply(reg, 0, 2);
    QuantumFourierTransform::inverse(reg, 0, 2);

    // Should recover original state
    const auto& state = reg.get_state();
    BOOST_TEST(std::abs(state[1].value().real() - 1.0) < 1e-10);
    BOOST_TEST(std::abs(state[0].value().real()) < 1e-10);
    BOOST_TEST(std::abs(state[2].value().real()) < 1e-10);
    BOOST_TEST(std::abs(state[3].value().real()) < 1e-10);
}

BOOST_AUTO_TEST_CASE(QuantumFourierTransform_error_handling_test) {
    QuantumRegister reg(2);

    // Test invalid ranges
    BOOST_CHECK_THROW(
        QuantumFourierTransform::apply(reg, 1, 1),
        std::invalid_argument
    );

    BOOST_CHECK_THROW(
        QuantumFourierTransform::apply(reg, 2, 3),
        std::invalid_argument
    );

    BOOST_CHECK_THROW(
        QuantumFourierTransform::inverse(reg, 1, 3),
        std::invalid_argument
    );
}

BOOST_AUTO_TEST_CASE(QuantumFourierTransform_phase_estimation_test) {
    QuantumRegister reg(3);

    // Initialize to uniform superposition
    for (size_t i = 0; i < 3; ++i) {
        reg.hadamard(i);
    }

    // Apply QFT
    QuantumFourierTransform::apply(reg, 0, 3);

    // Verify state is normalized
    const auto& state = reg.get_state();
    double total_prob = 0.0;
    for (const auto& amp : state) {
        total_prob += std::abs(amp.value()) * std::abs(amp.value());
    }
    BOOST_TEST(std::abs(total_prob - 1.0) < 1e-10);
}
