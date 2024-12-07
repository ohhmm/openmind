#pragma once

#include <vector>
#include <complex>
#include "Valuable.h"
#include "Variable.h"
#include "Integer.h"

namespace omnn::math {

class QuantumRegister : public Valuable {
public:
    using complex = std::complex<double>;
    using state_vector = std::vector<complex>;

    explicit QuantumRegister(size_t num_qubits);

    // Basic quantum gates
    void hadamard(size_t qubit);
    void phase(size_t qubit, double angle);
    void cnot(size_t control, size_t target);
    void swap(size_t qubit1, size_t qubit2);

    // Measurement
    Integer measure();
    Integer measure_range(size_t start, size_t end);

    // State manipulation
    const state_vector& get_state() const { return state; }
    size_t get_num_qubits() const { return num_qubits; }

    // Valuable interface implementation
    bool operator==(const Valuable& v) const override;
    size_t hash() const;

private:
    size_t num_qubits;
    state_vector state;

    // Helper functions
    void apply_single_qubit_gate(size_t qubit, const std::array<complex, 4>& matrix);
    void apply_controlled_gate(size_t control, size_t target, const std::array<complex, 4>& matrix);
};

} // namespace omnn::math
