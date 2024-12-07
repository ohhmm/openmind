#include "ModularExponentiation.h"
#include <vector>

namespace omnn::math {

void ModularExponentiation::apply(QuantumRegister& reg,
                                const Integer& base,
                                const Integer& modulus,
                                size_t input_start,
                                size_t input_end,
                                size_t output_start,
                                size_t output_end) {
    if (input_start >= input_end || output_start >= output_end ||
        input_end > reg.size() || output_end > reg.size()) {
        throw std::out_of_range("Invalid qubit range for modular exponentiation");
    }

    // Precompute powers of base modulo N
    std::vector<Integer> powers;
    Integer power = base % modulus;
    for (size_t i = input_start; i < input_end; ++i) {
        powers.push_back(power);
        power = (power * power) % modulus;
    }

    // Apply controlled modular multiplications
    for (size_t i = 0; i < input_end - input_start; ++i) {
        controlled_modular_multiply(reg, powers[i], modulus,
                                  input_start + i,
                                  output_start, output_end);
    }
}

void ModularExponentiation::controlled_modular_multiply(QuantumRegister& reg,
                                                      const Integer& factor,
                                                      const Integer& modulus,
                                                      size_t control,
                                                      size_t output_start,
                                                      size_t output_end) {
    // Apply controlled modular addition based on binary decomposition of factor
    Integer current_factor = factor;
    size_t width = output_end - output_start;

    for (size_t i = 0; i < width; ++i) {
        if ((current_factor % 2) == 1) {
            // Apply controlled modular addition
            reg.cnot(control, output_start + i);
        }
        current_factor = current_factor / 2;
    }
}

void ModularExponentiation::modular_add(QuantumRegister& reg,
                                      const Integer& value,
                                      const Integer& modulus,
                                      size_t start,
                                      size_t end) {
    // Implement modular addition using quantum arithmetic
    // This is a simplified version - in practice, we need to handle carries
    Integer current_value = value % modulus;
    size_t width = end - start;

    for (size_t i = 0; i < width; ++i) {
        if ((current_value % 2) == 1) {
            reg.x(start + i);
        }
        current_value = current_value / 2;
    }
}

} // namespace omnn::math
