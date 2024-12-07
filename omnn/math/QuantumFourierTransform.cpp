#include "QuantumFourierTransform.h"
#include <numbers>

namespace omnn::math {

void QuantumFourierTransform::apply(QuantumRegister& reg, size_t start, size_t end) {
    if (start >= end || end > reg.size()) {
        throw std::out_of_range("Invalid qubit range for QFT");
    }

    // Apply QFT
    for (size_t i = start; i < end; ++i) {
        // Apply Hadamard to current qubit
        reg.hadamard(i);

        // Apply controlled phase rotations
        for (size_t j = i + 1; j < end; ++j) {
            double angle = 2.0 * std::numbers::pi / (1ULL << (j - i + 1));
            controlled_phase(reg, j, i, angle);
        }
    }

    // Reverse the order of qubits in the specified range
    for (size_t i = start; i < start + (end - start) / 2; ++i) {
        size_t j = end - 1 - (i - start);
        // Swap qubits using CNOT gates
        reg.cnot(i, j);
        reg.cnot(j, i);
        reg.cnot(i, j);
    }
}

void QuantumFourierTransform::inverse(QuantumRegister& reg, size_t start, size_t end) {
    if (start >= end || end > reg.size()) {
        throw std::out_of_range("Invalid qubit range for inverse QFT");
    }

    // Reverse the order of qubits first
    for (size_t i = start; i < start + (end - start) / 2; ++i) {
        size_t j = end - 1 - (i - start);
        // Swap qubits using CNOT gates
        reg.cnot(i, j);
        reg.cnot(j, i);
        reg.cnot(i, j);
    }

    // Apply inverse QFT
    for (size_t i = end - 1; i >= start; --i) {
        // Apply inverse controlled phase rotations
        for (size_t j = end - 1; j > i; --j) {
            double angle = -2.0 * std::numbers::pi / (1ULL << (j - i + 1));
            controlled_phase(reg, j, i, angle);
        }

        // Apply Hadamard to current qubit
        reg.hadamard(i);

        if (i == start) break;  // Handle unsigned size_t underflow
    }
}

void QuantumFourierTransform::controlled_phase(QuantumRegister& reg, size_t control, size_t target, double angle) {
    if (control >= reg.size() || target >= reg.size()) {
        throw std::out_of_range("Qubit index out of range");
    }

    reg.phase(target, Valuable(angle));
}

} // namespace omnn::math
