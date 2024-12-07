#pragma once

#include "QuantumRegister.h"
#include "Integer.h"

namespace omnn::math {

class ModularExponentiation {
public:
    // Apply modular exponentiation operation: |x⟩|0⟩ → |x⟩|a^x mod N⟩
    static void apply(QuantumRegister& reg,
                     const Integer& a,
                     const Integer& N,
                     size_t input_start,
                     size_t input_end,
                     size_t output_start,
                     size_t output_end);

private:
    // Helper functions
    static void controlled_modular_multiplication(QuantumRegister& reg,
                                               const Integer& a,
                                               const Integer& N,
                                               size_t control,
                                               size_t output_start,
                                               size_t output_end);

    static void apply_controlled_addition(QuantumRegister& reg,
                                       const Integer& value,
                                       size_t control,
                                       size_t start,
                                       size_t end);
};

} // namespace omnn::math
