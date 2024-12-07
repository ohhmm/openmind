#include "QuantumFourierTransform.h"
#include <cmath>

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <math.h>
#endif

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

    const double sign = inverse ? -1.0 : 1.0;

    // Apply QFT/inverse QFT transformation
    for (size_t i = start; i < end; ++i) {
        // Apply Hadamard to current qubit
        reg.hadamard(i);

        // Apply controlled phase rotations
        for (size_t j = i + 1; j < end; ++j) {
            double angle = sign * M_PI / (1ULL << (j - i));
            controlled_phase(reg, j, i, angle);
        }
    }

    // Swap qubits if not inverse QFT
    if (!inverse) {
        for (size_t i = 0; i < (end - start) / 2; ++i) {
            reg.swap(start + i, end - 1 - i);
        }
    }
}

void QuantumFourierTransform::controlled_phase(QuantumRegister& reg,
                                            size_t control,
                                            size_t target,
                                            double angle) {
    if (control >= reg.get_num_qubits() || target >= reg.get_num_qubits()) {
        throw std::invalid_argument("Invalid control or target qubit");
    }

    // Apply controlled phase rotation
    reg.phase(target, angle);
}

} // namespace omnn::math
