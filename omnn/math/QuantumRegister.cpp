#include "QuantumRegister.h"
#include "QuantumFourierTransform.h"
#include <cmath>
#include <random>
#include <stdexcept>
#include <iostream>
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

void QuantumRegister::phase(size_t qubit, const Integer& numerator, const Integer& denominator) {
    Integer k = numerator % denominator;  // Normalize to range [0, denominator)
    if (k < 0) k += denominator;  // Ensure positive phase

    std::array<complex, 4> p_matrix;
    p_matrix[0] = complex(1.0, 0.0);
    p_matrix[1] = complex(0.0, 0.0);
    p_matrix[2] = complex(0.0, 0.0);
    p_matrix[3] = complex::from_exact_phase(k, denominator);

    apply_single_qubit_gate(qubit, p_matrix);
}





void QuantumRegister::controlled_controlled_phase(size_t control1, size_t control2, size_t target,
                                                  const Integer& numerator, const Integer& denominator) {
    if (control1 >= num_qubits || control2 >= num_qubits || target >= num_qubits ||
        control1 == control2 || control1 == target || control2 == target) {
        throw std::invalid_argument("Invalid control or target qubit");
    }

    // Calculate exact phase using Integer arithmetic
    Integer k = numerator % denominator;  // Normalize to range [0, denominator)
    if (k < 0) k += denominator;  // Ensure positive phase

    std::array<complex, 4> p_matrix;
    p_matrix[0] = complex(1.0, 0.0);
    p_matrix[1] = complex(0.0, 0.0);
    p_matrix[2] = complex(0.0, 0.0);
    p_matrix[3] = complex::from_exact_phase(k, denominator);

    apply_controlled_controlled_gate(control1, control2, target, p_matrix);
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

            complex a = state[i].value();
            complex b = state[i1].value();

            new_state[i] = complex(matrix[0].value() * a + matrix[1].value() * b);
            new_state[i1] = complex(matrix[2].value() * a + matrix[3].value() * b);
        }
    }

    double norm_factor = 0.0;
    const double epsilon = 1e-10;

    for (const auto& amp : new_state) {
        norm_factor += amp.norm();
    }

    if (norm_factor > epsilon) {
        const double scale = 1.0 / std::sqrt(norm_factor);
        for (auto& amp : new_state) {
            amp = complex(amp.value() * scale);
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

                complex a = state[i].value();
                complex b = state[i1].value();

                new_state[i] = complex(matrix[0].value() * a + matrix[1].value() * b);
                new_state[i1] = complex(matrix[2].value() * a + matrix[3].value() * b);
            }
        }
    }

    double norm_factor = 0.0;
    const double epsilon = 1e-10;

    for (const auto& amp : new_state) {
        norm_factor += amp.norm();
    }

    if (norm_factor > epsilon) {
        const double scale = 1.0 / std::sqrt(norm_factor);
        for (auto& amp : new_state) {
            amp = complex(amp.value() * scale);
        }
    }

    state = std::move(new_state);
}

Integer QuantumRegister::measure() {
    // Calculate cumulative probabilities
    std::vector<double> cumulative_probs;
    cumulative_probs.reserve(state.size());
    double total_prob = 0.0;

    for (const auto& amplitude : state) {
        total_prob += amplitude.norm();
        cumulative_probs.push_back(total_prob);
    }

    // Normalize probabilities
    if (total_prob > 1e-10) {
        for (auto& prob : cumulative_probs) {
            prob /= total_prob;
        }
    }

    // Generate random number between 0 and 1
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double rand_val = dist(gen);

    // Find the first state whose cumulative probability exceeds the random number
    for (size_t i = 0; i < cumulative_probs.size(); ++i) {
        if (rand_val <= cumulative_probs[i]) {
            // Collapse the state vector to the measured state
            state = state_vector(state.size());
            for (auto& amp : state) {
                amp = complex(0.0, 0.0);
            }
            state[i] = complex(1.0, 0.0);
            return Integer(i);
        }
    }

    // If we somehow get here (shouldn't happen due to normalization)
    return Integer(0);
}

Integer QuantumRegister::measure_range(size_t start, size_t end) {
    if (start >= num_qubits || end > num_qubits || start >= end) {
        throw std::invalid_argument("Invalid qubit range");
    }

    // Create a mask for the bits we want to measure
    size_t mask = ((1ULL << (end - start)) - 1) << start;

    // Calculate probabilities for each possible measurement outcome
    std::vector<double> probs(1ULL << (end - start), 0.0);

    for (size_t i = 0; i < state.size(); ++i) {
        size_t outcome = (i & mask) >> start;
        probs[outcome] += state[i].norm();
    }

    // Generate random number and find outcome
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double rand_val = dist(gen);

    double cumulative_prob = 0.0;
    size_t result = 0;

    for (size_t i = 0; i < probs.size(); ++i) {
        cumulative_prob += probs[i];
        if (rand_val <= cumulative_prob) {
            result = i;
            break;
        }
    }

    // Collapse state vector to consistent state with measurement
    state_vector new_state(state.size());
    for (auto& amp : new_state) {
        amp = complex(0.0, 0.0);
    }

    // Find all basis states consistent with measurement and preserve relative amplitudes
    double norm_factor = 0.0;
    for (size_t i = 0; i < state.size(); ++i) {
        if ((i & mask) >> start == result) {
            new_state[i] = state[i];
            norm_factor += state[i].norm();
        }
    }

    // Normalize the new state
    if (norm_factor > 1e-10) {
        double scale = 1.0 / std::sqrt(norm_factor);
        for (auto& amp : new_state) {
            amp = complex(amp.value() * scale);
        }
    }

    state = std::move(new_state);
    return Integer(result);
}

void QuantumRegister::apply_controlled_controlled_gate(size_t control1, size_t control2, size_t target,
                                                   const std::array<complex, 4>& matrix) {
    if (control1 >= num_qubits || control2 >= num_qubits || target >= num_qubits ||
        control1 == control2 || control1 == target || control2 == target) {
        throw std::invalid_argument("Invalid control or target qubit");
    }

    const size_t n = state.size();
    const size_t control1_mask = 1ULL << control1;
    const size_t control2_mask = 1ULL << control2;
    const size_t target_mask = 1ULL << target;
    state_vector new_state = state;

    // Process controlled-controlled operation with careful complex arithmetic
    for (size_t i = 0; i < n; ++i) {
        // Only apply gate when both control qubits are |1⟩
        if ((i & control1_mask) != 0 && (i & control2_mask) != 0) {
            if ((i & target_mask) == 0) {  // Only process when looking at |0⟩ state of target
                size_t i1 = i | target_mask;  // Corresponding |1⟩ state

                complex a = state[i].value();
                complex b = state[i1].value();

                new_state[i] = complex(matrix[0].value() * a + matrix[1].value() * b);
                new_state[i1] = complex(matrix[2].value() * a + matrix[3].value() * b);
            }
        }
    }

    // Normalize with careful floating-point handling
    double norm_factor = 0.0;
    const double epsilon = 1e-10;

    for (const auto& amp : new_state) {
        norm_factor += amp.norm();
    }

    if (norm_factor > epsilon) {
        const double scale = 1.0 / std::sqrt(norm_factor);
        for (auto& amp : new_state) {
            amp = complex(amp.value() * scale);
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

void QuantumRegister::controlled_modular_multiply(size_t control, size_t start, size_t end,
                                                const Integer& a, const Integer& N) {
    if (control >= num_qubits || start >= num_qubits || end > num_qubits || start >= end) {
        throw std::invalid_argument("Invalid control or register range");
    }

    // Apply QFT before multiplication
    qft_range(start, end);

    // Calculate phases for controlled addition
    const size_t width = end - start;
    const Integer mask = (Integer(1) << width) - 1;
    Integer current_power = Integer(1);

    // Apply controlled phase rotations for each bit position
    for (size_t i = 0; i < width; ++i) {
        Integer phase_factor = (current_power * a) % N;
        Integer numerator = phase_factor;
        Integer denominator = N;

        if (numerator < 0) numerator += denominator;

        controlled_phase(control, start + i, numerator, denominator);
        current_power = (current_power * Integer(2)) % N;
    }

    // Apply inverse QFT to complete multiplication
    inverse_qft_range(start, end);
}

void QuantumRegister::controlled_modular_add(size_t control, size_t start, size_t end,
                                           const Integer& a, const Integer& N) {
    if (control >= num_qubits || start >= num_qubits || end >= num_qubits || start >= end) {
        throw std::invalid_argument("Invalid control or register range");
    }

    QuantumFourierTransform::apply(*this, start, end);

    Integer shifted_a = a % N;
    for (size_t i = 0; i < end - start + 1; ++i) {
        for (size_t j = i; j < end - start + 1; ++j) {
            Integer phase_factor = (shifted_a << (end - start - j)) % N;
            Integer numerator = phase_factor;
            Integer denominator = N;

            if (numerator < 0) numerator += denominator;

            controlled_phase(control, start + i, numerator, denominator);
        }
    }

    QuantumFourierTransform::inverse(*this, start, end);
}

void QuantumRegister::qft_range(size_t start, size_t end) {
    if (start >= end || end > num_qubits) {
        throw std::invalid_argument("Invalid qubit range for QFT");
    }

    for (size_t i = start; i < end; ++i) {
        hadamard(i);
        for (size_t j = i + 1; j < end; ++j) {
            Integer numerator(1);
            Integer denominator = Integer(1) << (j - i);
            controlled_phase(i, j, numerator, denominator);
        }
    }

    for (size_t i = start; i < start + (end - start) / 2; ++i) {
        swap(i, end - 1 - (i - start));
    }
}

void QuantumRegister::inverse_qft_range(size_t start, size_t end) {
    if (start >= end || end > num_qubits) {
        throw std::invalid_argument("Invalid qubit range for inverse QFT");
    }

    for (size_t i = start; i < start + (end - start) / 2; ++i) {
        swap(i, end - 1 - (i - start));
    }

    for (size_t i = end - 1; i >= start; --i) {
        for (size_t j = i - 1; j >= start && j < end; --j) {
            Integer numerator(-1);
            Integer denominator = Integer(1) << (i - j);
            controlled_phase(j, i, numerator, denominator);
        }
        hadamard(i);

        if (i == start) break;
    }
}



void QuantumRegister::controlled_phase(size_t control, size_t target,
                                     const Integer& numerator, const Integer& denominator) {
    if (control >= num_qubits || target >= num_qubits) {
        throw std::invalid_argument("Invalid qubit indices");
    }

    // Calculate exact phase using Integer arithmetic as above
    Integer k = numerator % denominator;  // Normalize to range [0, denominator)
    if (k < 0) k += denominator;  // Ensure positive phase

    // Debug output for phase calculation
    std::cout << "\nApplying controlled phase rotation:" << std::endl;
    std::cout << "  Control qubit: " << control << ", Target qubit: " << target << std::endl;
    std::cout << "  Phase fraction: " << k << "/" << denominator << std::endl;
    std::cout << "  Quantum state before rotation:" << std::endl;
    for (size_t i = 0; i < state.size(); ++i) {
        if (std::abs(state[i].value()) > 1e-10) {
            std::cout << "    |" << std::bitset<8>(i) << ">: " << state[i] << std::endl;
        }
    }

    // Create phase matrix using exact Integer arithmetic
    std::array<complex, 4> p_matrix;
    p_matrix[0] = complex(1.0, 0.0);
    p_matrix[1] = complex(0.0, 0.0);
    p_matrix[2] = complex(0.0, 0.0);
    p_matrix[3] = complex::from_exact_phase(k, denominator);

    apply_controlled_gate(control, target, p_matrix);

    // Debug output after rotation
    std::cout << "  Quantum state after rotation:" << std::endl;
    for (size_t i = 0; i < state.size(); ++i) {
        if (std::abs(state[i].value()) > 1e-10) {
            std::cout << "    |" << std::bitset<8>(i) << ">: " << state[i] << std::endl;
        }
    }
}

} // namespace omnn::math
