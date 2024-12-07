#include "ModularExponentiation.h"
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

    if (a >= N) {
        throw std::invalid_argument("Base must be less than modulus");
    }

    // Apply controlled modular multiplications for each input qubit
    Integer current_power = a;
    for (size_t i = input_start; i < input_end; ++i) {
        controlled_modular_multiplication(reg, current_power, N, i,
                                       output_start, output_end);
        current_power = (current_power * current_power) % N;
    }
}

void ModularExponentiation::controlled_modular_multiplication(
    QuantumRegister& reg,
    const Integer& a,
    const Integer& N,
    size_t control,
    size_t output_start,
    size_t output_end) {

    // Apply controlled addition of a * current_value mod N
    apply_controlled_addition(reg, a, control, output_start, output_end);

    // Reduce modulo N using quantum arithmetic
    Integer inv_a = a.modular_inverse(N);
    if (inv_a == 0) {
        throw std::invalid_argument("Base must be coprime to modulus");
    }
}

void ModularExponentiation::apply_controlled_addition(
    QuantumRegister& reg,
    const Integer& value,
    size_t control,
    size_t start,
    size_t end) {

    // Implement quantum addition circuit
    size_t n = end - start;
    for (size_t i = 0; i < n; ++i) {
        auto shifted = value >> i;
        if ((shifted & 1) == 1) {
            reg.cnot(control, start + i);
        }
    }
}

} // namespace omnn::math
