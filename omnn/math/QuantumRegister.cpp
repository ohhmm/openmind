#include "QuantumRegister.h"
#include "Integer.h"
#include "Product.h"
#include <random>

namespace omnn::math {

QuantumRegister::QuantumRegister(size_t n_qubits)
    : num_qubits(n_qubits)
{
    // Initialize state vector with 2^n amplitudes
    const size_t dim = 1ULL << n_qubits;
    state = Extrapolator(dim, 1);

    // Initialize to |0...0⟩ state
    state(0, 0) = 1;
    for (size_t i = 1; i < dim; ++i) {
        state(i, 0) = 0;
    }
}

void QuantumRegister::hadamard(size_t qubit) {
    if (qubit >= num_qubits) {
        throw std::out_of_range("Qubit index out of range");
    }

    // H = 1/√2 * [1  1]
    //           [1 -1]
    const auto sqrt2 = std::sqrt(2.0);
    const size_t dim = 1ULL << num_qubits;
    Extrapolator new_state = state;

    for (size_t i = 0; i < dim; ++i) {
        if ((i & (1ULL << qubit)) == 0) {
            const size_t i1 = i | (1ULL << qubit);
            const auto v0 = state(i, 0);
            const auto v1 = state(i1, 0);
            new_state(i, 0) = (v0 + v1) / sqrt2;
            new_state(i1, 0) = (v0 - v1) / sqrt2;
        }
    }

    state = std::move(new_state);
}

void QuantumRegister::phase(size_t qubit, const Valuable& angle) {
    if (qubit >= num_qubits) {
        throw std::out_of_range("Qubit index out of range");
    }

    // Apply phase rotation
    const size_t dim = 1ULL << num_qubits;
    for (size_t i = 0; i < dim; ++i) {
        if (i & (1ULL << qubit)) {
            state(i, 0) *= Complex(std::cos(angle.AsDouble()), std::sin(angle.AsDouble()));
        }
    }
}

void QuantumRegister::cnot(size_t control, size_t target) {
    if (control >= num_qubits || target >= num_qubits) {
        throw std::out_of_range("Qubit index out of range");
    }

    const size_t dim = 1ULL << num_qubits;
    Extrapolator new_state = state;

    for (size_t i = 0; i < dim; ++i) {
        if (i & (1ULL << control)) {
            const size_t i_flipped = i ^ (1ULL << target);
            new_state(i, 0) = state(i_flipped, 0);
        } else {
            new_state(i, 0) = state(i, 0);
        }
    }

    state = std::move(new_state);
}

void QuantumRegister::x(size_t qubit) {
    if (qubit >= num_qubits) {
        throw std::out_of_range("Qubit index out of range");
    }

    const size_t dim = 1ULL << num_qubits;
    Extrapolator new_state = state;

    for (size_t i = 0; i < dim; ++i) {
        const size_t i_flipped = i ^ (1ULL << qubit);
        new_state(i, 0) = state(i_flipped, 0);
    }

    state = std::move(new_state);
}

void QuantumRegister::y(size_t qubit) {
    if (qubit >= num_qubits) {
        throw std::out_of_range("Qubit index out of range");
    }

    const size_t dim = 1ULL << num_qubits;
    Extrapolator new_state = state;
    const Complex i(0, 1);

    for (size_t i = 0; i < dim; ++i) {
        if ((i & (1ULL << qubit)) == 0) {
            const size_t i1 = i | (1ULL << qubit);
            new_state(i, 0) = -i * state(i1, 0);
            new_state(i1, 0) = i * state(i, 0);
        }
    }

    state = std::move(new_state);
}

void QuantumRegister::z(size_t qubit) {
    if (qubit >= num_qubits) {
        throw std::out_of_range("Qubit index out of range");
    }

    const size_t dim = 1ULL << num_qubits;
    for (size_t i = 0; i < dim; ++i) {
        if (i & (1ULL << qubit)) {
            state(i, 0) = -state(i, 0);
        }
    }
}

void QuantumRegister::reset() {
    const size_t dim = 1ULL << num_qubits;
    state(0, 0) = 1;
    for (size_t i = 1; i < dim; ++i) {
        state(i, 0) = 0;
    }
}

Valuable QuantumRegister::measure(size_t qubit) const {
    if (qubit >= num_qubits) {
        throw std::out_of_range("Qubit index out of range");
    }

    // Calculate probabilities
    const size_t dim = 1ULL << num_qubits;
    double prob_0 = 0.0;

    for (size_t i = 0; i < dim; ++i) {
        if ((i & (1ULL << qubit)) == 0) {
            const auto amp = state(i, 0);
            prob_0 += std::norm(Complex(amp.AsDouble()));
        }
    }

    // Random measurement
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    return Valuable(dis(gen) < prob_0 ? 0 : 1);
}

std::vector<Valuable> QuantumRegister::measure_all() const {
    std::vector<Valuable> results;
    results.reserve(num_qubits);

    for (size_t i = 0; i < num_qubits; ++i) {
        results.push_back(measure(i));
    }

    return results;
}

bool QuantumRegister::operator==(const QuantumRegister& other) const {
    return num_qubits == other.num_qubits && state == other.state;
}

QuantumRegister& QuantumRegister::operator=(const QuantumRegister& other) {
    if (this != &other) {
        num_qubits = other.num_qubits;
        state = other.state;
    }
    return *this;
}

void QuantumRegister::optimize() {
    state.optimize();
}

std::ostream& QuantumRegister::print(std::ostream& out) const {
    return out << "QuantumRegister(" << num_qubits << " qubits)";
}

Hash QuantumRegister::hash() const {
    Hash h;
    h << num_qubits;
    // Add state hash computation if needed
    return h;
}

} // namespace omnn::math
