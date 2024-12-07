#define BOOST_TEST_MODULE QuantumFourierTransform test
#include <boost/test/unit_test.hpp>
#include "../QuantumFourierTransform.h"
#include <cmath>
#include <numbers>

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(QFT_basic_test) {
    // Test QFT on a single qubit (should be equivalent to Hadamard)
    QuantumRegister qr(1);
    QuantumFourierTransform::apply(qr, 0, 1);

    const auto& state = qr.get_state();
    const double sqrt2 = std::sqrt(2.0);
    BOOST_TEST(state(0, 0).AsDouble() == 1.0/sqrt2);
    BOOST_TEST(state(1, 0).AsDouble() == 1.0/sqrt2);
}

BOOST_AUTO_TEST_CASE(QFT_two_qubit_test) {
    // Test QFT on two qubits
    QuantumRegister qr(2);

    // Prepare |01⟩ state
    qr.x(1);

    QuantumFourierTransform::apply(qr, 0, 2);

    const auto& state = qr.get_state();
    const double sqrt2 = std::sqrt(2.0);
    const double pi = std::numbers::pi;

    // Expected state after QFT:
    // |00⟩ -> 1/2
    // |01⟩ -> 1/2 * e^(i*pi/2)
    // |10⟩ -> 1/2 * e^(i*pi)
    // |11⟩ -> 1/2 * e^(3i*pi/2)

    BOOST_TEST(std::abs(state(0, 0).AsDouble() - 0.5) < 1e-10);
    BOOST_TEST(std::abs(state(1, 0).AsDouble() - 0.5) < 1e-10);
    BOOST_TEST(std::abs(state(2, 0).AsDouble() + 0.5) < 1e-10);
    BOOST_TEST(std::abs(state(3, 0).AsDouble() + 0.5) < 1e-10);
}

BOOST_AUTO_TEST_CASE(QFT_inverse_test) {
    // Test that QFT followed by inverse QFT returns to original state
    QuantumRegister qr(3);

    // Prepare non-trivial state
    qr.hadamard(0);
    qr.cnot(0, 1);
    qr.phase(2, Valuable(std::numbers::pi / 4));

    // Store initial state
    const auto initial_state = qr.get_state();

    // Apply QFT and then inverse QFT
    QuantumFourierTransform::apply(qr, 0, 3);
    QuantumFourierTransform::inverse(qr, 0, 3);

    // Compare with initial state
    const auto& final_state = qr.get_state();
    for (size_t i = 0; i < (1ULL << 3); ++i) {
        BOOST_TEST(std::abs(initial_state(i, 0).AsDouble() - final_state(i, 0).AsDouble()) < 1e-10);
    }
}

BOOST_AUTO_TEST_CASE(QFT_range_test) {
    // Test QFT on a subset of qubits
    QuantumRegister qr(4);

    // Apply QFT to middle two qubits only
    qr.x(1);  // Set second qubit to |1⟩
    QuantumFourierTransform::apply(qr, 1, 3);

    // First and last qubits should be unchanged
    const auto& state = qr.get_state();
    BOOST_TEST(state(0, 0).AsDouble() == 0);  // |0000⟩
    BOOST_TEST(std::abs(state(2, 0).AsDouble()) > 0);  // |0010⟩ should have non-zero amplitude
    BOOST_TEST(state(8, 0).AsDouble() == 0);  // |1000⟩
}
