#pragma once

#include "QuantumRegister.h"

namespace omnn::math {

class QuantumFourierTransform {
public:
    // Apply QFT to a range of qubits
    static void apply(QuantumRegister& reg, size_t start, size_t end);

    // Apply inverse QFT to a range of qubits
    static void inverse(QuantumRegister& reg, size_t start, size_t end);

private:
    // Helper functions
    static void apply_qft_internal(QuantumRegister& reg, size_t start, size_t end, bool inverse);
    static void controlled_phase(QuantumRegister& reg, size_t control, size_t target,
                                 const Integer& numerator, const Integer& denominator);  // Exact phase k/N
};

} // namespace omnn::math
