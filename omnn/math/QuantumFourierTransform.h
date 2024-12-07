#pragma once

#include "QuantumRegister.h"
#include <cmath>

namespace omnn::math {

class QuantumFourierTransform {
public:
    /**
     * Applies Quantum Fourier Transform to the specified range of qubits
     * @param reg Quantum register to transform
     * @param start First qubit in range (inclusive)
     * @param end Last qubit in range (exclusive)
     */
    static void apply(QuantumRegister& reg, size_t start, size_t end);

    /**
     * Applies inverse Quantum Fourier Transform to the specified range of qubits
     * @param reg Quantum register to transform
     * @param start First qubit in range (inclusive)
     * @param end Last qubit in range (exclusive)
     */
    static void inverse(QuantumRegister& reg, size_t start, size_t end);

private:
    // Helper function for controlled phase rotation
    static void controlled_phase(QuantumRegister& reg, size_t control, size_t target, double angle);
};

} // namespace omnn::math
