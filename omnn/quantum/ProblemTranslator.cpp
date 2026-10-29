#include "ProblemTranslator.h"
#include <omnn/math/Variable.h>
#include <sstream>
#include <iomanip>
#include <vector>

namespace omnn {
namespace quantum {

std::string ProblemTranslator::Translate(const omnn::math::Valuable& objective) {
    auto vaExps = objective.GetVaExps();
    if (vaExps.empty()) {
        throw std::runtime_error("The objective function must contain variables for Dirac acceleration");
    }

    // Use the first variable found to check if it's a polynomial
    const auto& firstVar = vaExps.begin()->first;
    if (!objective.IsPolynomial(firstVar)) {
        throw std::runtime_error("The objective function must be a polynomial for Dirac acceleration");
    }

    std::stringstream ss;
    ss << "{\"problem\": {\"coefficients\": [";

    bool first = true;
    for (auto const& [var, coeff] : vaExps) {
        if (!first) ss << ",";

        ss << "{\"vars\": [\"" << var.str() << "\"], \"coeff\": "
           << std::fixed << std::setprecision(6) << coeff.as<double>() << "}";
        first = false;
    }

    ss << "]}}";
    return ss.str();
}

} // namespace quantum
} // namespace omnn
