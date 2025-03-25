#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "Facility.h"
#include "FunctorFacility.h"

namespace omnn {
namespace math {
class Valuable;
class Variable;
}
}

/**
 * @brief LogicOffloadFacility provides a bridge between OpenMind and external logic systems
 * 
 * This facility allows OpenMind to offload logical deduction and inference to external
 * specialized systems. It extends FunctorFacility to provide a standardized interface
 * for connecting with third-party State-Transfer Machines (STMs) that implement logical
 * operations.
 */
class LogicOffloadFacility : public FunctorFacility {
public:
    using proposition_t = omnn::math::Valuable;
    using variable_t = omnn::math::Variable;
    using result_t = std::vector<proposition_t>;
    
    /**
     * @brief Callback type for external logic system results
     * 
     * This callback is invoked when the external logic system returns results
     * @param results Vector of propositions representing the logical deductions
     */
    using logic_result_callback_t = std::function<void(const result_t& results)>;
    
    /**
     * @brief Constructor for LogicOffloadFacility
     * 
     * @param connector Function that connects to the external logic system
     * @param callback Function to call when results are received
     */
    LogicOffloadFacility(
        std::function<bool()> connector,
        logic_result_callback_t callback
    );
    
    /**
     * @brief Add a proposition to be processed by the external logic system
     * 
     * @param proposition The logical proposition to add
     */
    void AddProposition(const proposition_t& proposition);
    
    /**
     * @brief Add a logical rule to be processed by the external logic system
     * 
     * @param antecedent The "if" part of the rule
     * @param consequent The "then" part of the rule
     */
    void AddRule(const proposition_t& antecedent, const proposition_t& consequent);
    
    /**
     * @brief Set the context for logical operations
     * 
     * @param context String identifier for the logical context
     */
    void SetContext(const std::string& context);
    
    /**
     * @brief Get the current results from the external logic system
     * 
     * @return Vector of propositions representing the logical deductions
     */
    result_t GetResults() const;
    
    /**
     * @brief Check if a proposition is satisfiable in the current context
     * 
     * @param proposition The proposition to check
     * @return true if satisfiable, false otherwise
     */
    bool IsSatisfiable(const proposition_t& proposition) const;
    
    /**
     * @brief Check if a proposition is a tautology in the current context
     * 
     * @param proposition The proposition to check
     * @return true if tautology, false otherwise
     */
    bool IsTautology(const proposition_t& proposition) const;
    
    /**
     * @brief Check if a proposition is a contradiction in the current context
     * 
     * @param proposition The proposition to check
     * @return true if contradiction, false otherwise
     */
    bool IsContradiction(const proposition_t& proposition) const;
    
private:
    std::vector<proposition_t> _propositions;
    std::vector<std::pair<proposition_t, proposition_t>> _rules;
    std::string _context;
    result_t _results;
    logic_result_callback_t _callback;
};
