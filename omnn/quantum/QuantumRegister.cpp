#include "QuantumRegister.h"
#include <cmath>
#include <complex>
#include <vector>

namespace omnn {
namespace math {

void QuantumRegister::apply_controlled_phase(size_t control, size_t target, double angle) {
    // Validate qubit indices
    if (control >= num_qubits || target >= num_qubits) {
        throw std::out_of_range("Qubit index out of range");
    }
    if (control == target) {
        throw std::invalid_argument("Control and target qubits must be different");
    }

    // Calculate controlled phase rotation
    const std::complex<double> phase_factor(std::cos(angle), std::sin(angle));

    // Size of the state vector
    const size_t n = 1ULL << num_qubits;

    // Apply controlled phase rotation
    for (size_t i = 0; i < n; ++i) {
        // Check if both control and target qubits are 1
        if ((i & (1ULL << control)) && (i & (1ULL << target))) {
            state_vector[i] *= phase_factor;
        }
    }

    // Normalize the state vector
    normalize();
}

void QuantumRegister::normalize() {
    double norm = 0.0;
    const double epsilon = 1e-10;  // Threshold for numerical stability

    // Calculate the norm
    for (const auto& amplitude : state_vector) {
        norm += std::norm(amplitude);
    }

    // Only normalize if the norm deviates significantly from 1
    if (std::abs(norm - 1.0) > epsilon) {
        const double scale_factor = 1.0 / std::sqrt(norm);
        for (auto& amplitude : state_vector) {
            amplitude *= scale_factor;
        }
    }
}

// PLACEHOLDER: Additional QuantumRegister implementation methods (constructors, other quantum gates, measurement, etc.)

} // namespace math
} // namespace omnn
