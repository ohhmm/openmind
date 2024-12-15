#pragma once

#include "QuantumRegister.h"
#include "Integer.h"
#include <optional>

namespace omnn::math {

class PeriodFinder {
public:
    // Find the period of function f(x) = a^x mod N
    static std::optional<Integer> findPeriod(const Integer& a, const Integer& N);

private:
    // Helper functions
    static std::optional<Integer> phaseEstimation(
        QuantumRegister& reg,
        const Integer& a,
        const Integer& N,
        size_t precision_qubits);

    static std::optional<Integer> phaseToPeriod(
        const Integer& phase_numerator,
        const Integer& phase_denominator,
        const Integer& N);

    static Integer continuedFractionExpansion(
        const Integer& numerator,
        const Integer& denominator,
        const Integer& bound);
};

} // namespace omnn::math
