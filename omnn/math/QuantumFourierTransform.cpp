#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include "QuantumFourierTransform.h"
#include <cmath>

namespace omnn::math {

void QuantumFourierTransform::apply(QuantumRegister& reg, size_t start, size_t end) {
    apply_qft_internal(reg, start, end, false);
}

void QuantumFourierTransform::inverse(QuantumRegister& reg, size_t start, size_t end) {
    apply_qft_internal(reg, start, end, true);
}

void QuantumFourierTransform::apply_qft_internal(QuantumRegister& reg,
                                               size_t start, size_t end,
                                               bool inverse) {
    if (start >= end || end > reg.get_num_qubits()) {
        throw std::invalid_argument("Invalid qubit range for QFT");
    }

    // Swap qubits first for inverse QFT
    if (inverse) {
        for (size_t i = 0; i < (end - start) / 2; ++i) {
            reg.swap(start + i, end - 1 - i);
        }
    }

    // Apply QFT operations
    for (size_t i = inverse ? end - 1 : start;
         inverse ? i != start - 1 : i < end;
         inverse ? --i : ++i) {

        // Apply Hadamard first
        reg.hadamard(i);

        // Apply controlled phase rotations
        for (size_t j = inverse ? i - 1 : i + 1;
             inverse ? j >= start && j < end : j < end;
             inverse ? --j : ++j) {

            // For inverse QFT, we need to negate the phase
            Integer numerator(inverse ? -1 : 1);
            Integer denominator = Integer(1) << (inverse ? i - j : j - i);

            // Control and target are swapped for inverse operation
            size_t control = inverse ? j : i;
            size_t target = inverse ? i : j;

            controlled_phase(reg, control, target, numerator, denominator);
        }
    }

    // Swap qubits after forward QFT
    if (!inverse) {
        for (size_t i = 0; i < (end - start) / 2; ++i) {
            reg.swap(start + i, end - 1 - i);
        }
    }
}

void QuantumFourierTransform::controlled_phase(QuantumRegister& reg,
                                            size_t control,
                                            size_t target,
                                            const Integer& numerator,
                                            const Integer& denominator) {
    if (control >= reg.get_num_qubits() || target >= reg.get_num_qubits()) {
        throw std::invalid_argument("Invalid control or target qubit");
    }

    reg.controlled_phase(control, target, numerator, denominator);
}

} // namespace omnn::math
