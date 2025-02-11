#include "Limit.h"
#include "Fraction.h"
#include "Exponentiation.h"
#include "NaN.h"
#include <omnn/rt/antiloop.hpp>

using namespace omnn::math;

std::ostream& Limit::print_sign(std::ostream& out) const {
    out << "->";
    return out;
}

std::ostream& Limit::print(std::ostream& out) const {
    out << "lim(" << limitVar << "->" << approachValue << ", " << _2 << ')';
    return out;
}

Limit::Limit(const std::string_view& str, std::shared_ptr<VarHost> host, bool itIsOptimized) {
    // Parse "lim(x->0, expr)" format
    // Store variable and approach value
    // Parse expression into _2
}

void Limit::optimize() {
    if (!optimizations && !IsSimple()) {
        hash = _1.Hash() ^ _2.Hash();
        return;
    }
    if (optimized) {
        return;
    }
    
    _2.optimize();
    
    // Will implement limit evaluation logic in next PR
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
        return limitVar == o.limitVar && 
               approachValue == o.approachValue && 
               _2 == o._2;
    }
    return false;
}

Limit::operator double() const {
    // Will implement conversion to double
    return 0.0;
}

Valuable& Limit::d(const Variable& x) {
    // Will implement derivative
    return *this;
}

Valuable& Limit::integral(const Variable& x, const Variable& C) {
    // Will implement integration
    return *this;
}
