#pragma once
#include <omnn/math/Valuable.h>
#include <omnn/math/Variable.h>
#include "DevinVarHost.h"
#include <memory>
#include <string>

namespace omnn {
namespace logic {

/**
 * DevinLogicInterface provides a simplified interface for integrating OpenMind's
 * logical operations and variable management into Devin's decision-making system.
 */
class DevinLogicInterface {
public:
    using ValueType = math::Valuable;
    using VariableType = math::Variable;
    
    /**
     * Creates a new logical variable with the given name
     */
    static VariableType createVariable(const std::string& name) {
        static auto host = std::make_shared<DevinVarHost>();
        auto var = VariableType(host);
        host->Host(std::make_any<std::string>(name));
        return var;
    }
    
    /**
     * Performs logical AND operation between two values
     */
    static ValueType logicalAnd(const ValueType& a, const ValueType& b) {
        return a.LogicAnd(b);
    }
    
    /**
     * Performs logical OR operation between two values
     */
    static ValueType logicalOr(const ValueType& a, const ValueType& b) {
        return a.LogicOr(b);
    }
    
    /**
     * Creates a logical equality constraint between two values
     */
    static ValueType equals(const ValueType& a, const ValueType& b) {
        return a.Equals(b);
    }
    
    /**
     * Evaluates if the given expression is valid under the current constraints
     */
    static bool evaluate(const ValueType& expr) {
        return static_cast<bool>(expr);
    }
    
    /**
     * Optimizes a logical expression for efficient evaluation
     */
    static ValueType optimize(ValueType expr) {
        expr.optimize();
        return expr;
    }
};

}} // namespace omnn::logic
