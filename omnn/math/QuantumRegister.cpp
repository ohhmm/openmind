#include "QuantumRegister.h"
#include <random>
#include <cmath>

namespace omnn::math {

QuantumRegister::QuantumRegister(size_t num_qubits)
    : num_qubits(num_qubits), state(1ULL << num_qubits) {
    // Initialize to |0...0⟩
    state[0] = complex(1.0, 0.0);
}

void QuantumRegister::hadamard(size_t qubit) {
    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    std::array<complex, 4> h_matrix = {
        complex(inv_sqrt2, 0), complex(inv_sqrt2, 0),
        complex(inv_sqrt2, 0), complex(-inv_sqrt2, 0)
    };
    apply_single_qubit_gate(qubit, h_matrix);
}

void QuantumRegister::phase(size_t qubit, double angle) {
    std::array<complex, 4> p_matrix = {
        complex(1.0, 0.0), complex(0.0, 0.0),
        complex(0.0, 0.0), std::polar(1.0, angle)
    };
    apply_single_qubit_gate(qubit, p_matrix);
}

void QuantumRegister::cnot(size_t control, size_t target) {
    std::array<complex, 4> x_matrix = {
        complex(0.0, 0.0), complex(1.0, 0.0),
        complex(1.0, 0.0), complex(0.0, 0.0)
    };
    apply_controlled_gate(control, target, x_matrix);
}

void QuantumRegister::swap(size_t qubit1, size_t qubit2) {
    cnot(qubit1, qubit2);
    cnot(qubit2, qubit1);
    cnot(qubit1, qubit2);
}

Integer QuantumRegister::measure() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    double rand = dist(gen);
    double cumulative_prob = 0.0;

    for (size_t i = 0; i < state.size(); ++i) {
        cumulative_prob += std::norm(state[i]);
        if (rand <= cumulative_prob) {
            // Collapse state
            state = state_vector(state.size());
            state[i] = complex(1.0, 0.0);
            return Integer(i);
        }
    }

    // Should never reach here if state is normalized
    return Integer(0);
}

Integer QuantumRegister::measure_range(size_t start, size_t end) {
    if (start >= num_qubits || end > num_qubits || start >= end) {
        throw std::invalid_argument("Invalid qubit range");
    }

    auto result = measure();
    size_t mask = ((1ULL << (end - start)) - 1) << start;
    return Integer((result.to_size_t() & mask) >> start);
}

void QuantumRegister::apply_single_qubit_gate(size_t qubit,
                                            const std::array<complex, 4>& matrix) {
    if (qubit >= num_qubits) {
        throw std::invalid_argument("Invalid qubit index");
    }

    const size_t n = state.size();
    const size_t mask = 1ULL << qubit;
    state_vector new_state(n);


    for (size_t i = 0; i < n; ++i) {
        if ((i & mask) == 0) {
            size_t i1 = i | mask;
            new_state[i] = matrix[0] * state[i] + matrix[1] * state[i1];
            new_state[i1] = matrix[2] * state[i] + matrix[3] * state[i1];
        }
    }

    state = std::move(new_state);
}

void QuantumRegister::apply_controlled_gate(size_t control, size_t target,
                                          const std::array<complex, 4>& matrix) {
    if (control >= num_qubits || target >= num_qubits || control == target) {
        throw std::invalid_argument("Invalid control or target qubit");
    }

    const size_t n = state.size();
    const size_t control_mask = 1ULL << control;
    const size_t target_mask = 1ULL << target;
    state_vector new_state = state;

    for (size_t i = 0; i < n; ++i) {
        if ((i & control_mask) != 0) {
            if ((i & target_mask) == 0) {
                size_t i1 = i | target_mask;
                new_state[i] = matrix[0] * state[i] + matrix[1] * state[i1];
                new_state[i1] = matrix[2] * state[i] + matrix[3] * state[i1];
            }
        }
    }

    state = std::move(new_state);
}

// Valuable interface implementation
bool QuantumRegister::operator==(const Valuable& v) const {
    if (auto* other = dynamic_cast<const QuantumRegister*>(&v)) {
        return num_qubits == other->num_qubits && state == other->state;
    }
    return false;
}

size_t QuantumRegister::hash() const {
    size_t h = std::hash<size_t>{}(num_qubits);
    for (const auto& s : state) {
        h ^= std::hash<double>{}(s.real());
        h ^= std::hash<double>{}(s.imag());
    }
    return h;
}

} // namespace omnn::math
