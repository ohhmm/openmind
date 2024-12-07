#define BOOST_TEST_MODULE ModularExponentiation test
#include <boost/test/unit_test.hpp>
#include "../ModularExponentiation.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(ModularExponentiation_basic_test) {
    // Test simple modular exponentiation: 2^3 mod 5 = 3
    QuantumRegister qr(6);  // 3 qubits for input, 3 for output

    // Prepare input state |3⟩ (binary 011)
    qr.x(0);
    qr.x(1);

    ModularExponentiation::apply(qr, Integer(2), Integer(5), 0, 3, 3, 6);

    // Measure output register - should be in state |3⟩ (2^3 mod 5 = 3)
    auto result = qr.measure_all();

    // Check output bits (last 3 qubits)
    BOOST_TEST(result[3].AsInt() == 1);  // LSB of 3
    BOOST_TEST(result[4].AsInt() == 1);  // Middle bit of 3
    BOOST_TEST(result[5].AsInt() == 0);  // MSB of 3
}

BOOST_AUTO_TEST_CASE(ModularExponentiation_zero_test) {
    // Test with input |0⟩: a^0 mod N = 1
    QuantumRegister qr(4);  // 2 qubits each for input and output

    ModularExponentiation::apply(qr, Integer(7), Integer(9), 0, 2, 2, 4);

    // Measure output register - should be in state |1⟩
    auto result = qr.measure_all();

    // Check output bits
    BOOST_TEST(result[2].AsInt() == 1);  // LSB of 1
    BOOST_TEST(result[3].AsInt() == 0);  // MSB of 1
}

BOOST_AUTO_TEST_CASE(ModularExponentiation_large_numbers_test) {
    // Test with larger numbers: 3^5 mod 7 = 5
    QuantumRegister qr(8);  // 4 qubits each for input and output

    // Prepare input state |5⟩ (binary 0101)
    qr.x(0);
    qr.x(2);

    ModularExponentiation::apply(qr, Integer(3), Integer(7), 0, 4, 4, 8);

    // Measure output register - should be in state |5⟩
    auto result = qr.measure_all();

    // Check output bits
    BOOST_TEST(result[4].AsInt() == 1);  // LSB of 5
    BOOST_TEST(result[5].AsInt() == 0);
    BOOST_TEST(result[6].AsInt() == 1);
    BOOST_TEST(result[7].AsInt() == 0);  // MSB of 5
}

BOOST_AUTO_TEST_CASE(ModularExponentiation_error_handling_test) {
    QuantumRegister qr(4);

    // Test invalid qubit ranges
    BOOST_CHECK_THROW(
        ModularExponentiation::apply(qr, Integer(2), Integer(5), 0, 5, 2, 4),
        std::out_of_range
    );

    BOOST_CHECK_THROW(
        ModularExponentiation::apply(qr, Integer(2), Integer(5), 2, 1, 2, 4),
        std::out_of_range
    );
}
