#pragma once

#include "ValuableDescendantContract.h"
#include "../extrapolator/Extrapolator.h"
#include "Complex.h"
#include <vector>
#include <cmath>

namespace omnn::math {

/**
 * Quantum register implementation using matrix operations for quantum state manipulation.
 * Implements basic quantum gates and state management.
 */
class QuantumRegister : public ValuableDescendantContract<QuantumRegister> {
    using base = ValuableDescendantContract<QuantumRegister>;

    Extrapolator state;  // Stores quantum state as complex amplitudes
    size_t num_qubits;   // Number of qubits in register

public:
    explicit QuantumRegister(size_t n_qubits);

    // Basic quantum gates
    void hadamard(size_t qubit);
    void phase(size_t qubit, const Valuable& angle);
    void cnot(size_t control, size_t target);
    void x(size_t qubit);  // Pauli-X gate
    void y(size_t qubit);  // Pauli-Y gate
    void z(size_t qubit);  // Pauli-Z gate

    // State manipulation
    void reset();
    Valuable measure(size_t qubit) const;
    std::vector<Valuable> measure_all() const;

    // Required by ValuableDescendantContract
    bool operator==(const QuantumRegister& other) const override;
    QuantumRegister& operator=(const QuantumRegister& other);
    void optimize() override;
    std::ostream& print(std::ostream& out) const override;
    Hash hash() const override;

    // Getters
    size_t size() const { return num_qubits; }
    const Extrapolator& get_state() const { return state; }
};

} // namespace omnn::math
