#include "QuantumRegister.h"
#include <random>
#include <cmath>
#include <bitset>

namespace omnn::math {

QuantumRegister::QuantumRegister(size_t num_qubits)
    : num_qubits(num_qubits), state(1ULL << num_qubits) {
    state[0] = complex(1.0, 0.0);
}

void QuantumRegister::x(size_t qubit) {
    std::array<complex, 4> x_matrix = {
        complex(0.0, 0.0), complex(1.0, 0.0),
        complex(1.0, 0.0), complex(0.0, 0.0)
    };
    apply_single_qubit_gate(qubit, x_matrix);
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
        complex(0.0, 0.0), complex(std::cos(angle), std::sin(angle))
    };
    apply_single_qubit_gate(qubit, p_matrix);
}

void QuantumRegister::controlled_phase(size_t control, size_t target, double angle) {
    if (control >= num_qubits || target >= num_qubits || control == target) {
        throw std::invalid_argument("Invalid control or target qubit");
    }

    std::array<complex, 4> p_matrix = {
        complex(1.0, 0.0), complex(0.0, 0.0),
        complex(0.0, 0.0), complex(std::cos(angle), std::sin(angle))
    };
    apply_controlled_gate(control, target, p_matrix);
}

void QuantumRegister::controlled_hadamard(size_t control, size_t target) {
    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    std::array<complex, 4> h_matrix = {
        complex(inv_sqrt2, 0), complex(inv_sqrt2, 0),
        complex(inv_sqrt2, 0), complex(-inv_sqrt2, 0)
    };
    apply_controlled_gate(control, target, h_matrix);
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
        cumulative_prob += state[i].norm();
        if (rand <= cumulative_prob) {
            state = state_vector(state.size());
            state[i] = complex(1.0, 0.0);
            return Integer(i);
        }
    }

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

    double norm_factor = 0.0;
    for (size_t i = 0; i < n; i += 2) {
        size_t i0 = i & ~mask;
        size_t i1 = i0 | mask;

        complex v0 = matrix[0].value() * state[i0].value() + matrix[1].value() * state[i1].value();
        complex v1 = matrix[2].value() * state[i0].value() + matrix[3].value() * state[i1].value();

        new_state[i0] = Complex(v0);
        new_state[i1] = Complex(v1);

        norm_factor += v0.norm() + v1.norm();
    }

    if (norm_factor > 0.0) {
        const double scale = 1.0 / std::sqrt(norm_factor);
        for (size_t i = 0; i < n; ++i) {
            new_state[i] = Complex(new_state[i].value() * scale);
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

    double norm_factor = 0.0;
    for (size_t i = 0; i < n; ++i) {
        if ((i & control_mask) != 0) {
            size_t i0 = i & ~target_mask;
            size_t i1 = i | target_mask;

            complex v0 = matrix[0].value() * state[i0].value() + matrix[1].value() * state[i1].value();
            complex v1 = matrix[2].value() * state[i0].value() + matrix[3].value() * state[i1].value();

            new_state[i0] = Complex(v0);
            new_state[i1] = Complex(v1);

            norm_factor += v0.norm() + v1.norm();
        } else {
            norm_factor += state[i].norm();
        }
    }

    if (norm_factor > 0.0) {
        const double scale = 1.0 / std::sqrt(norm_factor);
        for (size_t i = 0; i < n; ++i) {
            new_state[i] = Complex(new_state[i].value() * scale);
        }
    }

    state = std::move(new_state);
}

bool QuantumRegister::operator==(const Valuable& v) const {
    if (auto* other = dynamic_cast<const QuantumRegister*>(&v)) {
        return num_qubits == other->num_qubits && state == other->state;
    }
    return false;
}

size_t QuantumRegister::getHash() const {
    size_t h = std::hash<size_t>{}(num_qubits);
    for (const auto& s : state) {
        h ^= std::hash<double>{}(s.real());
        h ^= std::hash<double>{}(s.imag());
    }
    return h;
}

void QuantumRegister::print_state() const {
    std::cout << "Quantum Register State (" << num_qubits << " qubits):\n";
    for (size_t i = 0; i < state.size(); ++i) {
        if (state[i].norm() > 1e-10) {
            std::cout << "|" << std::bitset<32>(i).to_string().substr(32 - num_qubits)
                     << "> : " << state[i] << "\n";
        }
    }
    std::cout << std::endl;
}

} // namespace omnn::math
