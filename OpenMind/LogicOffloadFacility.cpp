#include "stdafx.h"
#include "LogicOffloadFacility.h"
#include <omnn/math/Valuable.h>
#include <omnn/math/Variable.h>

LogicOffloadFacility::LogicOffloadFacility(
    std::function<bool()> connector,
    logic_result_callback_t callback
) : FunctorFacility(connector), _callback(callback) {
}

void LogicOffloadFacility::AddProposition(const proposition_t& proposition) {
    _propositions.push_back(proposition);
}

void LogicOffloadFacility::AddRule(const proposition_t& antecedent, const proposition_t& consequent) {
    _rules.emplace_back(antecedent, consequent);
}

void LogicOffloadFacility::SetContext(const std::string& context) {
    _context = context;
}

LogicOffloadFacility::result_t LogicOffloadFacility::GetResults() const {
    return _results;
}

bool LogicOffloadFacility::IsSatisfiable(const proposition_t& proposition) const {
    // This would typically call into the external logic system
    // For now, we provide a simple implementation
    return !IsContradiction(proposition);
}

bool LogicOffloadFacility::IsTautology(const proposition_t& proposition) const {
    // A proposition is a tautology if its negation is a contradiction
    omnn::math::Valuable negation = proposition.operator!();
    return IsContradiction(negation);
}

bool LogicOffloadFacility::IsContradiction(const proposition_t& proposition) const {
    // This would typically call into the external logic system
    // For now, we provide a simple implementation that checks if the proposition
    // evaluates to a constant false
    
    // Check if the proposition is equivalent to false (0)
    return proposition.IsZero();
}
