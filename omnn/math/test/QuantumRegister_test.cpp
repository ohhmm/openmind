#define BOOST_TEST_MODULE QuantumRegister test
#include <boost/test/unit_test.hpp>
#include "../QuantumRegister.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(QuantumRegister_initialization_test) {
    QuantumRegister qr(2);
    BOOST_TEST(qr.size() == 2);

    // Check initial state |00⟩
    const auto& state = qr.get_state();
    BOOST_TEST(state(0, 0) == 1);  // |00⟩ amplitude
    BOOST_TEST(state(1, 0) == 0);  // |01⟩ amplitude
    BOOST_TEST(state(2, 0) == 0);  // |10⟩ amplitude
    BOOST_TEST(state(3, 0) == 0);  // |11⟩ amplitude
}

BOOST_AUTO_TEST_CASE(QuantumRegister_hadamard_test) {
    QuantumRegister qr(1);
    qr.hadamard(0);

    const auto& state = qr.get_state();
    const double sqrt2 = std::sqrt(2.0);
    BOOST_TEST(state(0, 0).AsDouble() == 1.0/sqrt2);
    BOOST_TEST(state(1, 0).AsDouble() == 1.0/sqrt2);
}

BOOST_AUTO_TEST_CASE(QuantumRegister_cnot_test) {
    QuantumRegister qr(2);

    // Prepare |10⟩ state
    qr.x(0);

    // Apply CNOT
    qr.cnot(0, 1);

    const auto& state = qr.get_state();
    BOOST_TEST(state(0, 0) == 0);  // |00⟩ amplitude
    BOOST_TEST(state(1, 0) == 0);  // |01⟩ amplitude
    BOOST_TEST(state(2, 0) == 0);  // |10⟩ amplitude
    BOOST_TEST(state(3, 0) == 1);  // |11⟩ amplitude
}

BOOST_AUTO_TEST_CASE(QuantumRegister_measurement_test) {
    QuantumRegister qr(1);

    // Prepare |0⟩ state
    auto result = qr.measure(0);
    BOOST_TEST(result == 0);

    // Prepare |1⟩ state
    qr.x(0);
    result = qr.measure(0);
    BOOST_TEST(result == 1);
}

BOOST_AUTO_TEST_CASE(QuantumRegister_optimization_test) {
    QuantumRegister qr(2);
    qr.optimize();  // Should not throw

    // Test state consistency after optimization
    const auto& state = qr.get_state();
    BOOST_TEST(state(0, 0) == 1);
    BOOST_TEST(state(1, 0) == 0);
    BOOST_TEST(state(2, 0) == 0);
    BOOST_TEST(state(3, 0) == 0);
}
