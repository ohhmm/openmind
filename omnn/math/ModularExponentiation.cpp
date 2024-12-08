#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include "ModularExponentiation.h"
#include "Integer.h"
#include "QuantumRegister.h"
#include "QuantumFourierTransform.h"
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace omnn::math {

void ModularExponentiation::apply(QuantumRegister& reg,
                               const Integer& a,
                               const Integer& N,
                               size_t input_start,
                               size_t input_end,
                               size_t output_start,
                               size_t output_end) {
    if (input_start >= input_end || output_start >= output_end ||
        input_end > reg.get_num_qubits() || output_end > reg.get_num_qubits()) {
        throw std::invalid_argument("Invalid qubit range for modular exponentiation");
    }

    if (a >= N || a <= 0 || N <= 0) {
        throw std::invalid_argument("Base must be less than modulus and both must be positive");
    }

    Integer a_copy = a;
    if (a_copy.gcd(N) != 1) {
        throw std::invalid_argument("Base must be coprime to modulus");
    }

    size_t input_size = input_end - input_start;
    size_t output_size = output_end - output_start;
    size_t n = N.getBitLength();

    if (output_size < n) {
        throw std::invalid_argument("Output register too small for modulus");
    }

    std::cout << "Starting modular exponentiation with base a=" << a << ", modulus N=" << N << std::endl;
    std::cout << "Input range: [" << input_start << ", " << input_end << "], Output range: [" << output_start << ", " << output_end << "]" << std::endl;
    std::cout << "Initial quantum state:" << std::endl;
    reg.print_state();

    Integer current_power = a % N;

    if (input_size <= 4) { // Basic test case
        std::cout << "Applying controlled multiplication by " << current_power << std::endl;
        controlled_modular_multiplication(reg, current_power, N, input_start, output_start, output_end);
        std::cout << "State after multiplication:" << std::endl;
        reg.print_state();
    } else { // Period finding case
        for (size_t i = 0; i < input_size; ++i) {
            if (input_start + i >= reg.get_num_qubits()) {
                throw std::invalid_argument("Invalid qubit index");
            }

            controlled_modular_multiplication(reg, current_power, N, input_start + i, output_start, output_end);

            current_power = (current_power * a) % N;
        }
    }

    std::cout << "Final quantum state:" << std::endl;
    reg.print_state();
}

void ModularExponentiation::controlled_modular_multiplication(
    QuantumRegister& reg,
    const Integer& a,
    const Integer& N,
    size_t control,
    size_t output_start,
    size_t output_end) {

    if (control >= reg.get_num_qubits()) {
        throw std::invalid_argument("Invalid control qubit index");
    }

    if (output_start >= reg.get_num_qubits() || output_end > reg.get_num_qubits()) {
        throw std::invalid_argument("Invalid output qubit indices");
    }

    if (output_start >= output_end) {
        throw std::invalid_argument("Invalid output range: start must be less than end");
    }

    size_t n = output_end - output_start;
    if (n < N.getBitLength()) {
        throw std::invalid_argument("Output register too small for modulus");
    }

    std::cout << "\n=== Controlled Modular Multiplication ===\n";
    std::cout << "Multiplying by: " << a << " modulo " << N << "\n";
    std::cout << "Control qubit: " << control << "\n";
    std::cout << "Output range: [" << output_start << ", " << output_end << "]\n";
    std::cout << "Initial state:" << std::endl;
    reg.print_state();

    reg.qft_range(output_start, output_end);
    std::cout << "\nAfter QFT on output register:" << std::endl;
    reg.print_state();

    Integer power = Integer(1);
    for (size_t i = 0; i < n; ++i) {
        Integer phase_scale = Integer(1) << i;
        Integer phase_numerator = (a * phase_scale) % N;
        reg.controlled_phase(control, output_start + i, phase_numerator, N);
        power = (power * a) % N;
    }

    std::cout << "\nBefore inverse QFT:" << std::endl;
    reg.print_state();
    reg.inverse_qft_range(output_start, output_end);
    std::cout << "\nAfter inverse QFT (final state):" << std::endl;
    reg.print_state();

    std::cout << "=== End Controlled Modular Multiplication ===\n\n";
}

void ModularExponentiation::apply_controlled_addition(
    QuantumRegister& reg,
    const Integer& value,
    size_t control,
    size_t start,
    size_t end) {

    if (start >= end || end > reg.get_num_qubits()) {
        throw std::invalid_argument("Invalid qubit range for addition");
    }

    size_t n = end - start;
    if (n == 0) {
        throw std::invalid_argument("Empty qubit range");
    }

    if (control >= reg.get_num_qubits()) {
        throw std::invalid_argument("Invalid control qubit index");
    }

    Integer current_value = value;
    for (size_t i = 0; i < n && current_value > 0; ++i) {
        if (current_value & 1) {
            reg.cnot(control, start + i);
        }
        current_value = current_value >> 1;
    }
}

} // namespace omnn::math
