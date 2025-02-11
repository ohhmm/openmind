#include "Limit.h"
#include "Fraction.h"
#include "Exponentiation.h"
#include "NaN.h"
#include <omnn/rt/antiloop.hpp>
#include <omnn/rt/strhash.hpp>

using namespace omnn::math;


std::ostream& Limit::print(std::ostream& out) const {
    out << "lim(" << variable << "->" << _1 << ", " << _2 << ')';
    return out;
}

std::ostream& Limit::print_sign(std::ostream& out) const {
    out << "->";
    return out;
}

Limit::Limit(const std::string_view& str, std::shared_ptr<VarHost> host, bool itIsOptimized) {
    // Parse "lim(x->0, expr)" format
    auto arrow = str.find("->");
    if (arrow == std::string::npos) {
        LOG_AND_IMPLEMENT("Invalid limit format: missing ->");
    }

    auto comma = str.find(',', arrow);
    if (comma == std::string::npos) {
        LOG_AND_IMPLEMENT("Invalid limit format: missing comma");
    }

    // Extract and trim variable and approach value strings
    auto varStr = str.substr(0, arrow);
    rt::Trim(varStr);
    auto approachStr = str.substr(arrow + 2, comma - (arrow + 2));
    rt::Trim(approachStr);
    auto exprStr = str.substr(comma + 1);
    rt::Trim(exprStr);

    if (varStr.empty() || approachStr.empty() || exprStr.empty()) {
        LOG_AND_IMPLEMENT("Empty components in limit expression");
    }

    // Parse components
    auto variable = Valuable(varStr, host, itIsOptimized);
    variable = std::move(variable.as<Variable>());
    // Variable name is set through VarHost
    _1 = Valuable(approachStr, host, itIsOptimized);
    _2 = Valuable(exprStr, host, itIsOptimized);

    hash = variable.Hash() ^ _1.Hash() ^ _2.Hash();
    optimized = itIsOptimized;
}

void Limit::optimize() {

    if (!optimizations && !IsSimple()) {
        hash = variable.Hash() ^ _1.Hash() ^ _2.Hash();
        return;
    }
    if (optimized) {
        return;
    }
    
    _2.optimize();

    // Evaluate limit by substituting values approaching limitVar
    auto expr = _2;
    auto delta = Fraction(1, 1000); // Small value for approximation
    
    // Evaluate at approach value + delta and - delta
    auto evalPlus = expr;
    auto evalMinus = expr;
    
    evalPlus.Eval(variable, _1 + delta);
    evalMinus.Eval(variable, _1 - delta);
    
    evalPlus.optimize();
    evalMinus.optimize();
    
    // If values are close enough, we've found the limit
    if (evalPlus == evalMinus) {
        Become(std::move(evalPlus));
    } else {
        // Handle special cases (0/0, ∞/∞)
        if (evalPlus.IsZero() && evalMinus.IsZero()) {
            // Use L'Hôpital's rule
            auto num = expr.d(variable);
            auto den = expr.d(variable);
            num.optimize();
            den.optimize();
            if (!den.IsZero()) {
                Become(Fraction(std::move(num), std::move(den)));
            }
        }
    }
    
    optimized = true;
}

max_exp_t Limit::getMaxVaExp(const Valuable& _1, const Valuable& _2) {
    return _2.getMaxVaExp();
}

Valuable::vars_cont_t Limit::GetVaExps() const {
    Valuable::vars_cont_t exponentiations;
    for (auto& variable : Vars()) {
        exponentiations.emplace(std::move(variable), NaN());
    }
    return exponentiations;
}

bool Limit::operator==(const Valuable& other) const {
    if (other.Is<Limit>()) {
        auto& o = other.as<Limit>();
        return variable == o.variable && 
               _1 == o._1 && 
               _2 == o._2;
    }
    return false;
}

Limit::operator double() const {
    auto result = *this;
    result.optimize();
    return static_cast<double>(result);
}

Valuable& Limit::d(const Variable& x) {
    // Derivative of a limit is the limit of the derivative
    auto expr = _2;
    expr.d(x);
    Limit result(std::move(variable), std::move(_1));
    result._2 = std::move(expr);
    return Become(std::move(result));
}

Valuable& Limit::integral(const Variable& x, const Variable& C) {
    // Integral of a limit is the limit of the integral
    auto expr = _2;
    expr.integral(x, C);
    Limit result(std::move(variable), std::move(_1));
    result._2 = std::move(expr);
    return Become(std::move(result));
}
