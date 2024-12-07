#include "PeriodFinder.h"
#include <cmath>
#include <numbers>
#include "ContinuedFraction.h"

namespace omnn::math {

std::optional<Integer> PeriodFinder::findPeriod(const Integer& a, const Integer& N) {
    if (a >= N || a <= 1) {
        throw std::invalid_argument("Base 'a' must be between 1 and N");
    }

    // Calculate required number of qubits
    size_t precision_qubits = calculateRequiredQubits(N);
    size_t n = N.getBitLength();
    size_t total_qubits = precision_qubits + 2 * n;

    // Create quantum register
    QuantumRegister reg(total_qubits);

    // Initialize input register to superposition
    for (size_t i = 0; i < precision_qubits; ++i) {
        reg.hadamard(i);
    }

    // Perform quantum phase estimation
    auto phase = phaseEstimation(reg, a, N, precision_qubits);
    if (!phase) {
        return std::nullopt;
    }

    // Convert phase to period
    return phaseToPeriod(phase->numerator(), phase->denominator(), N);
}

size_t PeriodFinder::calculateRequiredQubits(const Integer& N) {
    // Number of precision qubits needed for phase estimation
    // We need enough precision to distinguish 1/r where r is the period
    return 2 * N.getBitLength() + 3;
}

std::optional<Integer> PeriodFinder::phaseEstimation(
    QuantumRegister& reg,
    const Integer& a,
    const Integer& N,
    size_t precision_qubits) {

    size_t n = N.getBitLength();
    size_t output_start = precision_qubits;
    size_t output_end = precision_qubits + n;

    // Apply controlled modular exponentiation
    ModularExponentiation::apply(reg, a, N, 0, precision_qubits,
                               output_start, output_end);

    // Apply inverse QFT to the precision qubits
    QuantumFourierTransform::inverse(reg, 0, precision_qubits);

    // Measure precision qubits to get phase
    std::vector<Valuable> measurements;
    for (size_t i = 0; i < precision_qubits; ++i) {
        measurements.push_back(reg.measure(i));
    }

    // Convert measurements to phase
    Integer phase_numerator = 0;
    for (size_t i = 0; i < precision_qubits; ++i) {
        if (measurements[i].AsInt() == 1) {
            phase_numerator += Integer(1) << (precision_qubits - 1 - i);
        }
    }

    return std::make_optional<Integer>(phase_numerator);
}

std::optional<Integer> PeriodFinder::phaseToPeriod(
    const Integer& phase_numerator,
    const Integer& phase_denominator,
    const Integer& N) {

    // Use continued fraction expansion to find the period
    auto fraction = ContinuedFraction::fromRational(phase_numerator, phase_denominator);
    auto convergents = fraction.getConvergents();

    // Find the first convergent that gives a valid period
    for (const auto& convergent : convergents) {
        Integer candidate_period = convergent.denominator();


        if (candidate_period > 0 && candidate_period < N) {
            // Verify that this is actually the period
            Integer power = 1;
            for (Integer i = 0; i < candidate_period; ++i) {
                power = (power * phase_numerator) % N;
            }

            if (power == 1) {
                return std::make_optional(candidate_period);
            }
        }
    }

    return std::nullopt;
}

} // namespace omnn::math
