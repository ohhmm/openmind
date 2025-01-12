#include "Logarithm.h"

#include "Exponentiation.h"
#include "Fraction.h"

#include <cmath>
#include <boost/math/special_functions/log1p.hpp>

namespace omnn {
namespace math {

    max_exp_t Logarithm::getMaxVaExp(const Valuable& _1, const Valuable&) {
        return _1.getMaxVaExp();
    }

    std::ostream& Logarithm::print(std::ostream& out) const
    {
        out << "log(" << _1 << ", " << _2 << ')';
        return out;
    }

    std::ostream& Logarithm::print_sign(std::ostream& out) const
    {
        out << 'l';
        return out;
    }

    void Logarithm::optimize()
    {
        // Simplify logarithm if the target is a power of the base
        if (_2.IsExponentiation() && _2.as<Exponentiation>().getBase() == _1) {
            Become(std::move(_2.as<Exponentiation>().eexp()));
        }
        // Simplify logarithm if base and target are the same
        else if (_1 == _2) {
            Become(1);
        }
        // Add more simplification rules as needed
    }

    Valuable& Logarithm::operator +=(const Valuable& v)
    {
        // Implement addition of logarithms
        if (v.IsLogarithm() && v.as<Logarithm>().getBase() == _1) {
            // If bases are the same, add the targets
            _2 += v.as<Logarithm>().getTarget();
            optimize();
        } else {
            // If bases are different, cannot simplify further
            base::operator+=(v);
        }
        return *this;
    }

    Valuable& Logarithm::operator *=(const Valuable& v)
    {
        // Implement multiplication of logarithms
        if (v.IsLogarithm() && v.as<Logarithm>().getBase() == _1) {
            // If bases are the same, multiply the targets
            _2 *= v.as<Logarithm>().getTarget();
            optimize();
        } else {
            // If bases are different, cannot simplify further
            base::operator*=(v);
        }
        return *this;
    }

    bool Logarithm::MultiplyIfSimplifiable(const Valuable& v)
    {
        // Check if multiplication can be simplified
        if (v.IsLogarithm() && v.as<Logarithm>().getBase() == _1) {
            // If bases are the same, multiply the targets and simplify
            _2 *= v.as<Logarithm>().getTarget();
            optimize();
            return true;
        }
        return false;
    }

    std::pair<bool, Valuable> Logarithm::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        // Check if multiplication can be simplified and return the result
        if (v.IsLogarithm() && v.as<Logarithm>().getBase() == _1) {
            // If bases are the same, return true and the multiplied targets
            Valuable newTarget = _2 * v.as<Logarithm>().getTarget();
            return {true, Logarithm(_1, newTarget)};
        }
        return {false, *this};
    }

    bool Logarithm::SumIfSimplifiable(const Valuable& v)
    {
        // Check if summation can be simplified
        if (v.IsLogarithm() && v.as<Logarithm>().getBase() == _1) {
            // If bases are the same, add the targets and simplify
            _2 += v.as<Logarithm>().getTarget();
            optimize();
            return true;
        }
        return false;
    }

    std::pair<bool, Valuable> Logarithm::IsSummationSimplifiable(const Valuable& v) const
    {
        // Check if summation can be simplified and return the result
        if (v.IsLogarithm() && v.as<Logarithm>().getBase() == _1) {
            // If bases are the same, return true and the added targets
            Valuable newTarget = _2 + v.as<Logarithm>().getTarget();
            return {true, Logarithm(_1, newTarget)};
        }
        return {false, *this};
    }

    Valuable& Logarithm::operator/=(const Valuable& value) {
        return Become(Fraction{*this, value});
    }

    Valuable& Logarithm::operator^=(const Valuable& value) {
	return Become(Exponentiation{*this, value}); }

    Logarithm::operator double() const {
        IMPLEMENT
    }

    Valuable& Logarithm::d(const Variable& x) {
        IMPLEMENT
        return *this;
    }

    Valuable& Logarithm::integral(const Variable& x, const Variable& C) {
        IMPLEMENT
        return *this;
    }

}  // namespace math
}  // namespace omnn
