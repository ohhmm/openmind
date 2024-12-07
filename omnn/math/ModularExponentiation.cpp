#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include "ModularExponentiation.h"
#include <stdexcept>
#include <vector>
#include <iostream>

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
    std::cout << "Input range: [" << input_start << ", " << input_end << "]" << std::endl;
    std::cout << "Output range: [" << output_start << ", " << output_end << "]" << std::endl;

    Integer current_power = a;
    for (size_t i = 0; i < input_size; ++i) {
        if (input_start + i >= reg.get_num_qubits()) {
            throw std::invalid_argument("Invalid qubit index");
        }

        std::cout << "Iteration " << i << ": current_power = " << current_power << std::endl;

        controlled_modular_multiplication(reg, current_power, N, input_start + i,
                                      output_start, output_end);
        current_power = (current_power * current_power) % N;
        std::cout << "After iteration " << i << ": next_power = " << current_power << std::endl;
    }
}

void ModularExponentiation::controlled_modular_multiplication(
    QuantumRegister& reg,
    const Integer& a,
    const Integer& N,
    size_t control,
    size_t output_start,
    size_t output_end) {

    if (output_start >= output_end || output_end > reg.get_num_qubits()) {
        throw std::invalid_argument("Invalid output range");
    }

    size_t n = output_end - output_start;
    if (n < N.getBitLength()) {
        throw std::invalid_argument("Output register too small for modulus");
    }

    if (control >= reg.get_num_qubits()) {
        throw std::invalid_argument("Invalid control qubit index");
    }

    std::cout << "\n=== Controlled Modular Multiplication ===\n";
    std::cout << "Parameters: a = " << a << ", N = " << N << "\n";
    std::cout << "Quantum register range: [" << output_start << ", " << output_end << "]\n";
    std::cout << "Initial quantum register state:\n";
    reg.print_state();

    std::cout << "\nStarting quantum arithmetic phase...\n";
    std::cout << "Initial state before any operations:\n";
    reg.print_state();

    for (size_t i = 0; i < n; ++i) {
        reg.controlled_hadamard(control, output_start + i);
        std::cout << "After controlled-Hadamard on qubit " << output_start + i << ":\n";
        reg.print_state();
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            Integer shifted = (a << j) % N;
            if ((shifted >> i) & 1) {
                double phase = M_PI / (1 << (n - j));
                reg.controlled_phase(control, output_start + i, phase);
                std::cout << "Applied controlled phase " << phase
                         << " between control=" << control
                         << " and target=" << (output_start + i)
                         << " for shifted=" << shifted << "\n";
                reg.print_state();
            }
        }
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            reg.controlled_phase(output_start + j, output_start + i, -M_PI / (1 << (j - i)));
        }
        reg.controlled_hadamard(control, output_start + i);
    }
    std::cout << "After controlled inverse QFT:\n";
    reg.print_state();

    std::cout << "\nFinal quantum register state:\n";
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

    for (size_t i = 0; i < n; ++i) {
        if ((value >> i) & 1) {
            reg.cnot(control, start + i);
        }
    }
}

} // namespace omnn::math
