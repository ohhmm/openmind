#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <cmath>
#endif

#define BOOST_TEST_MODULE ModularExponentiation test
#include <boost/test/unit_test.hpp>
#include "../ModularExponentiation.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(ModularExponentiation_basic_test) {
    QuantumRegister reg(6);  // 2 input + 4 output qubits

    // Initialize input register to |1⟩
    reg.hadamard(0);
    reg.phase(0, M_PI);
    reg.hadamard(0);

    // Apply modular exponentiation: a = 2, N = 15
    ModularExponentiation::apply(reg, Integer(2), Integer(15), 0, 2, 2, 6);

    // Measure output register
    auto result = reg.measure_range(2, 6);
    BOOST_TEST(result == Integer(2));  // 2^1 mod 15 = 2
}

BOOST_AUTO_TEST_CASE(ModularExponentiation_error_handling_test) {
    QuantumRegister reg(4);

    // Test invalid ranges
    BOOST_CHECK_THROW(
        ModularExponentiation::apply(reg, Integer(2), Integer(15), 0, 5, 2, 4),
        std::invalid_argument
    );

    // Test invalid base
    BOOST_CHECK_THROW(
        ModularExponentiation::apply(reg, Integer(15), Integer(15), 0, 2, 2, 4),
        std::invalid_argument
    );
}

BOOST_AUTO_TEST_CASE(ModularExponentiation_superposition_test) {
    QuantumRegister reg(6);  // 2 input + 4 output qubits

    // Create superposition in input register
    reg.hadamard(0);

    // Apply modular exponentiation: a = 7, N = 15
    ModularExponentiation::apply(reg, Integer(7), Integer(15), 0, 2, 2, 6);

    // Verify quantum state is valid
    const auto& state = reg.get_state();
    double total_prob = 0.0;
    for (const auto& amp : state) {
        total_prob += std::norm(amp);
    }
    BOOST_TEST(std::abs(total_prob - 1.0) < 1e-10);
}

BOOST_AUTO_TEST_CASE(ModularExponentiation_period_test) {
    QuantumRegister reg(8);  // 3 input + 5 output qubits

    // Initialize input register to |1⟩
    reg.hadamard(0);
    reg.phase(0, M_PI);
    reg.hadamard(0);

    // Apply modular exponentiation: a = 2, N = 21
    ModularExponentiation::apply(reg, Integer(2), Integer(21), 0, 3, 3, 8);

    // Verify period properties
    auto result = reg.measure_range(3, 8);
    BOOST_TEST(result == Integer(2));  // 2^1 mod 21 = 2

    // Reset and test next power
    reg = QuantumRegister(8);
    reg.hadamard(0);
    reg.hadamard(1);
    reg.phase(1, M_PI);
    reg.hadamard(1);

    ModularExponentiation::apply(reg, Integer(2), Integer(21), 0, 3, 3, 8);
    result = reg.measure_range(3, 8);
    BOOST_TEST(result == Integer(4));  // 2^2 mod 21 = 4
}
