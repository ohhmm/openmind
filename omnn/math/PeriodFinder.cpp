#include "PeriodFinder.h"
#include "QuantumFourierTransform.h"
#include "ModularExponentiation.h"
#include <cmath>

namespace omnn::math {

std::optional<Integer> PeriodFinder::findPeriod(const Integer& a, const Integer& N) {
    if (a >= N || a <= 0 || N <= 0) {
        throw std::invalid_argument("Invalid input for period finding");
    }

    // Calculate required number of qubits
    size_t n = N.getBitLength();
    size_t precision_qubits = 2 * n + 3;  // For good probability of success

    // Create quantum register
    QuantumRegister reg(precision_qubits + 2 * n);

    // Try phase estimation multiple times
    for (int attempt = 0; attempt < 5; ++attempt) {
        auto period = phaseEstimation(reg, a, N, precision_qubits);
        if (period && *period > 0 && *period < N) {
            // Verify period
            Integer test = 1;
            for (Integer i = 0; i < *period; ++i) {
                test = (test * a) % N;
            }
            if (test == 1) {
                return period;
            }
        }
    }

    return std::nullopt;
}

std::optional<Integer> PeriodFinder::phaseEstimation(
    QuantumRegister& reg,
    const Integer& a,
    const Integer& N,
    size_t precision_qubits) {

    // Initialize input register to superposition
    for (size_t i = 0; i < precision_qubits; ++i) {
        reg.hadamard(i);
    }

    // Apply modular exponentiation
    ModularExponentiation::apply(reg, a, N, 0, precision_qubits,
                               precision_qubits, precision_qubits + N.getBitLength());

    // Apply inverse QFT
    QuantumFourierTransform::inverse(reg, 0, precision_qubits);

    // Measure phase register
    auto phase = reg.measure_range(0, precision_qubits);
    if (phase == 0) {
        return std::nullopt;
    }

    // Convert phase to period
    return phaseToPeriod(phase, Integer(1) << precision_qubits, N);
}

std::optional<Integer> PeriodFinder::phaseToPeriod(
    const Integer& phase_numerator,
    const Integer& phase_denominator,
    const Integer& N) {

    // Use continued fraction expansion to find period
    auto period = continuedFractionExpansion(phase_numerator,
                                           phase_denominator,
                                           N);

    return period > 0 ? std::optional<Integer>(period) : std::nullopt;
}

Integer PeriodFinder::continuedFractionExpansion(
    const Integer& numerator,
    const Integer& denominator,
    const Integer& bound) {

    Integer n = numerator;
    Integer d = denominator;
    Integer a, q, t;
    Integer prev_convergent_n = 1, convergent_n = 0;
    Integer prev_convergent_d = 0, convergent_d = 1;

    while (d != 0) {
        q = n / d;
        t = n % d;
        n = d;
        d = t;

        t = convergent_n;
        convergent_n = q * convergent_n + prev_convergent_n;
        prev_convergent_n = t;

        t = convergent_d;
        convergent_d = q * convergent_d + prev_convergent_d;
        prev_convergent_d = t;

        if (convergent_d >= bound) {
            break;
        }
    }

    return convergent_d;
}

} // namespace omnn::math
