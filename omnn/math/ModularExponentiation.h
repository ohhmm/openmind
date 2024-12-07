#pragma once

#include "QuantumRegister.h"
#include "Integer.h"
#include "Valuable.h"

namespace omnn::math {

class ModularExponentiation {
public:
    /**
     * Applies modular exponentiation operation: |x⟩|0⟩ -> |x⟩|a^x mod N⟩
     * @param reg Quantum register containing input and output registers
     * @param base Base number a
     * @param modulus Modulus N
     * @param input_start Start of input register
     * @param input_end End of input register
     * @param output_start Start of output register
     * @param output_end End of output register
     */
    static void apply(QuantumRegister& reg,
                     const Integer& base,
                     const Integer& modulus,
                     size_t input_start,
                     size_t input_end,
                     size_t output_start,
                     size_t output_end);

private:
    // Helper function to apply controlled modular multiplication
    static void controlled_modular_multiply(QuantumRegister& reg,
                                          const Integer& factor,
                                          const Integer& modulus,
                                          size_t control,
                                          size_t output_start,
                                          size_t output_end);

    // Helper function to apply modular addition
    static void modular_add(QuantumRegister& reg,
                           const Integer& value,
                           const Integer& modulus,
                           size_t start,
                           size_t end);
};

} // namespace omnn::math
