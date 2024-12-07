#define BOOST_TEST_MODULE QuantumRegister test
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <boost/test/unit_test.hpp>
#include "../QuantumRegister.h"
#include <cmath>

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(QuantumRegister_initialization_test) {
    QuantumRegister reg(2);
    const auto& state = reg.get_state();

    BOOST_TEST(state.size() == 4);
    BOOST_TEST(std::abs(state[0] - std::complex<double>(1, 0)) < 1e-10);
    for (size_t i = 1; i < 4; ++i) {
        BOOST_TEST(std::abs(state[i]) < 1e-10);
    }
}

BOOST_AUTO_TEST_CASE(QuantumRegister_hadamard_test) {
    QuantumRegister reg(1);
    reg.hadamard(0);

    const auto& state = reg.get_state();
    double inv_sqrt2 = 1.0 / std::sqrt(2.0);

    BOOST_TEST(std::abs(state[0] - std::complex<double>(inv_sqrt2, 0)) < 1e-10);
    BOOST_TEST(std::abs(state[1] - std::complex<double>(inv_sqrt2, 0)) < 1e-10);
}

BOOST_AUTO_TEST_CASE(QuantumRegister_cnot_test) {
    QuantumRegister reg(2);
    reg.hadamard(0);
    reg.cnot(0, 1);

    const auto& state = reg.get_state();
    double inv_sqrt2 = 1.0 / std::sqrt(2.0);

    BOOST_TEST(std::abs(state[0] - std::complex<double>(inv_sqrt2, 0)) < 1e-10);
    BOOST_TEST(std::abs(state[3] - std::complex<double>(inv_sqrt2, 0)) < 1e-10);
    BOOST_TEST(std::abs(state[1]) < 1e-10);
    BOOST_TEST(std::abs(state[2]) < 1e-10);
}

BOOST_AUTO_TEST_CASE(QuantumRegister_measurement_test) {
    const int num_trials = 1000;
    const double tolerance = 0.1;

    QuantumRegister reg(1);
    reg.hadamard(0);

    int zeros = 0;
    for (int i = 0; i < num_trials; ++i) {
        QuantumRegister test_reg = reg;
        auto result = test_reg.measure();
        if (result == Integer(0)) {
            ++zeros;
        }
    }

    double zero_prob = static_cast<double>(zeros) / num_trials;
    BOOST_TEST(std::abs(zero_prob - 0.5) < tolerance);
}

BOOST_AUTO_TEST_CASE(QuantumRegister_phase_test) {
    QuantumRegister reg(1);
    reg.hadamard(0);
    reg.phase(0, M_PI);

    const auto& state = reg.get_state();
    double inv_sqrt2 = 1.0 / std::sqrt(2.0);

    BOOST_TEST(std::abs(state[0] - std::complex<double>(inv_sqrt2, 0)) < 1e-10);
    BOOST_TEST(std::abs(state[1] - std::complex<double>(-inv_sqrt2, 0)) < 1e-10);
}
