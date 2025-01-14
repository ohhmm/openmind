#include "pi.h"
#include "Exponentiation.h"
#include "Fraction.h"
#include "NaN.h"
#include <numbers>

using namespace omnn::math;

constinit std::string_view Pi::SerializationName = "pi";

bool Pi::operator<(const Valuable& v) const {
    if (v.Is_pi()) {
        return false;  // π = π, so π < π is false
    }
    // Handle numeric types (integers and fractions)
    if (v.IsInt() || v.IsFraction()) {
        // Special case: if comparing with itself or another π, use direct comparison
        if (v.Is_pi()) {
            return false;  // π is never less than itself
        }
        
        // Convert to double for comparison, but handle negative numbers carefully
        auto dval = static_cast<double>(v);
        if (dval <= 0) {
            return false;  // π is positive, never less than non-positive numbers
        }
        return std::numbers::pi < dval;
    }
    return base::operator<(v);
}

Pi::operator double() const { 
    return std::numbers::pi; 
}

Valuable Pi::operator^(const Valuable& exp) const {
    if (exp.IsInt() || exp.IsFraction()) {
        if (exp.IsZero()) {
            return Valuable(1);  // π^0 = 1
        }
        if (exp.IsInt() && exp == constants::one) {
            return *this;  // π^1 = π
        }
        auto val = std::numbers::pi;
        auto power = static_cast<double>(exp);
        if (power < 0) {
            val = 1.0 / val;
            power = -power;
        }
        auto result = Valuable(std::pow(val, power));
        result.optimize();  // Let the system handle optimization
        return result;
    }
    // For non-numeric exponents, create an Exponentiation
    return Valuable(Exponentiation(*this, exp));
}

Valuable Pi::operator/(const Valuable& v) const {
    if (v.IsInt() || v.IsFraction()) {
        if (v.IsZero()) {
            return Valuable(omnn::math::NaN());  // Division by zero
        }
        if (v.IsInt() && v == constants::one) {
            return *this;  // π/1 = π
        }
        auto result = Valuable(std::numbers::pi / static_cast<double>(v));
        result.optimize();  // Let the system handle optimization
        return result;
    }
    // For non-numeric divisors, create a Fraction
    return Valuable(Fraction(*this, v));
}
