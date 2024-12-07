#pragma once

#include <vector>
#include <array>
#include <bitset>
#include <iostream>
#include "Complex.h"
#include "Valuable.h"
#include "Variable.h"
#include "Integer.h"

namespace omnn::math {

class QuantumRegister : public Valuable {
public:
    using complex = Complex;
    using state_vector = std::vector<complex>;

    explicit QuantumRegister(size_t num_qubits);

    // Basic quantum gates
    void x(size_t qubit);  // NOT gate
    void hadamard(size_t qubit);
    void phase(size_t qubit, const Integer& numerator, const Integer& denominator);  // Exact phase k/N
    void controlled_phase(size_t control, size_t target, const Integer& numerator, const Integer& denominator);  // Exact phase k/N
    void controlled_controlled_phase(size_t control1, size_t control2, size_t target, const Integer& numerator, const Integer& denominator);  // Exact phase k/N
    void controlled_hadamard(size_t control, size_t target);
    void cnot(size_t control, size_t target);
    void swap(size_t qubit1, size_t qubit2);

    // Quantum arithmetic operations
    void controlled_modular_multiply(size_t control, size_t start, size_t end,
                                     const Integer& a, const Integer& N);
    void controlled_modular_add(size_t control, size_t start, size_t end,
                                const Integer& a, const Integer& N);

    // Quantum Fourier Transform operations
    void qft_range(size_t start, size_t end);
    void inverse_qft_range(size_t start, size_t end);

    // Measurement
    Integer measure();
    Integer measure_range(size_t start, size_t end);

    // State manipulation
    const state_vector& get_state() const { return state; }
    size_t get_num_qubits() const { return num_qubits; }
    void print_state() const;

    // Valuable interface implementation
    bool operator==(const Valuable& v) const override;

protected:
    size_t getHash() const;

private:
    size_t num_qubits;
    state_vector state;

    // Helper functions
    void apply_single_qubit_gate(size_t qubit, const std::array<complex, 4>& matrix);
    void apply_controlled_gate(size_t control, size_t target, const std::array<complex, 4>& matrix);
    void apply_controlled_controlled_gate(size_t control1, size_t control2, size_t target, const std::array<complex, 4>& matrix);
};

} // namespace omnn::math
