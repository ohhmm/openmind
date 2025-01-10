#pragma once

#include <complex>
#include <vector>
#include <stdexcept>

namespace omnn {
namespace math {

class QuantumRegister {
public:
    // Constructor for n-qubit register initialized to |0...0⟩ state
    explicit QuantumRegister(size_t num_qubits)
        : num_qubits(num_qubits), state_vector(1ULL << num_qubits) {
        // Initialize to |0...0⟩ state
        state_vector[0] = std::complex<double>(1.0, 0.0);
    }

    // Apply controlled phase rotation
    void apply_controlled_phase(size_t control, size_t target, double angle);

    // Normalize the quantum state
    void normalize();

private:
    size_t num_qubits;
    std::vector<std::complex<double>> state_vector;
};

} // namespace math
} // namespace omnn
