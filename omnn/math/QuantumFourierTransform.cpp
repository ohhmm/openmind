#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include "QuantumFourierTransform.h"
#include <cmath>
#include <iostream>

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
    std::cout << "\n=== " << (inverse ? "Inverse" : "Forward") << " QFT Debug ===" << std::endl;
    std::cout << "Initial state:" << std::endl;
    const auto& initial_state = reg.get_state();
    for (size_t k = 0; k < initial_state.size(); ++k) {
        std::cout << "  |" << k << ">: " << initial_state[k].value() << std::endl;
    }

    if (inverse) {
        for (size_t i = 0; i < (end - start) / 2; ++i) {
            reg.swap(start + i, end - 1 - i);
            std::cout << "Swapped qubits " << start + i << " and " << end - 1 - i << std::endl;
        }
    }

    if (inverse) {
        // Process qubits in reverse order for inverse QFT
        for (size_t i = end - 1; i != start - 1; --i) {
            std::cout << "Processing qubit " << i << std::endl;

            // Apply Hadamard first in inverse QFT
            std::cout << "  Applying Hadamard to qubit " << i << std::endl;
            reg.hadamard(i);

            // Apply controlled phase rotations in reverse order
            for (size_t j = i - 1; j >= start && j < end; --j) {
                Integer numerator(sign);
                Integer denominator = Integer(1) << (i - j);
                std::cout << "  " << (inverse ? "Inverse: " : "Forward: ") << "Applying controlled phase rotation between qubits "
                         << j << " and " << i << " with angle " << numerator << "/" << denominator << std::endl;
                controlled_phase(reg, j, i, numerator, denominator);
            }

            const auto& state = reg.get_state();
            std::cout << "  Current state:" << std::endl;
            for (size_t k = 0; k < state.size(); ++k) {
                std::cout << "    |" << k << ">: " << state[k].value() << std::endl;
            }
        }
    } else {
        for (size_t i = start; i < end; ++i) {
            std::cout << "Processing qubit " << i << std::endl;

            std::cout << "  Applying Hadamard to qubit " << i << std::endl;
            reg.hadamard(i);

            for (size_t j = i + 1; j < end; ++j) {
                Integer numerator(sign);
                Integer denominator = Integer(1) << (j - i);
                std::cout << "  " << (inverse ? "Inverse: " : "Forward: ") << "Applying controlled phase rotation between qubits "
                         << i << " and " << j << " with angle " << numerator << "/" << denominator << std::endl;
                controlled_phase(reg, i, j, numerator, denominator);
            }

            const auto& state = reg.get_state();
            std::cout << "  Current state:" << std::endl;
            for (size_t k = 0; k < state.size(); ++k) {
                std::cout << "    |" << k << ">: " << state[k].value() << std::endl;
            }
        }
    }

    if (!inverse) {
        for (size_t i = 0; i < (end - start) / 2; ++i) {
            reg.swap(start + i, end - 1 - i);
            std::cout << "Swapped qubits " << start + i << " and " << end - 1 - i << std::endl;
        }
    }
}

void QuantumFourierTransform::controlled_phase(QuantumRegister& reg,
                                            size_t control,
                                            size_t target,
                                            const Integer& numerator,
                                            const Integer& denominator) {
    if (control >= reg.get_num_qubits() || target >= reg.get_num_qubits()) {
        std::cout << "Invalid control (" << control << ") or target (" << target
                 << ") qubit for register size " << reg.get_num_qubits() << std::endl;
        throw std::invalid_argument("Invalid control or target qubit");
    }

    reg.controlled_phase(control, target, numerator, denominator);
}

} // namespace omnn::math
