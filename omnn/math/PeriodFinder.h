#pragma once

#include "QuantumRegister.h"
#include "QuantumFourierTransform.h"
#include "ModularExponentiation.h"
#include "Integer.h"
#include <optional>

namespace omnn::math {

class PeriodFinder {
public:
    /**
     * Finds the period of the function f(x) = a^x mod N
     * @param a Base number
     * @param N Modulus
     * @return Period if found, std::nullopt if period finding fails
     */
    static std::optional<Integer> findPeriod(const Integer& a, const Integer& N);

private:
    // Helper function to determine required number of qubits
    static size_t calculateRequiredQubits(const Integer& N);

    // Helper function to perform quantum phase estimation
    static std::optional<Integer> phaseEstimation(
        QuantumRegister& reg,
        const Integer& a,
        const Integer& N,
        size_t precision_qubits);

    // Helper function to convert phase to period
    static std::optional<Integer> phaseToPeriod(
        const Integer& phase_numerator,
        const Integer& phase_denominator,
        const Integer& N);
};

} // namespace omnn::math
