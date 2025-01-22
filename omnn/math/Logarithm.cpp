#include "Logarithm.h"

#include "Exponentiation.h"
#include "Fraction.h"
#include "Euler.h"

#include "rt/strhash.hpp"

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

    Logarithm::Logarithm(const std::string_view& str, std::shared_ptr<VarHost> host, bool itIsOptimized) {
        auto comma = str.find(',');
        auto isLn = comma == std::string::npos;

        // Extract and trim base and target strings
        auto baseStr = str.substr(0, comma);
        rt::Trim(baseStr);
        auto targetStr = str.substr(comma + 1);
        rt::Trim(targetStr);

        auto baseIsEmpty = !isLn && baseStr.empty();
        if (baseIsEmpty && targetStr.empty()) {
            LOG_AND_IMPLEMENT("Empty base and target during logarithm parsing");
        } else if (baseIsEmpty) {
            LOG_AND_IMPLEMENT("Empty base in logarithm: Log(" << baseStr << ", " << targetStr << ")");
        } else if (targetStr.empty()) {
            LOG_AND_IMPLEMENT("Empty target in logarithm: Log(" << baseStr << ", " << targetStr << ")");
        }

        auto base = isLn ? Euler() : Valuable(baseStr, host, itIsOptimized);
        setBase(std::move(base));
        setTarget(Valuable(targetStr, host, itIsOptimized));

        Valuable::hash = _1.Hash() ^ _2.Hash();
        Valuable::optimized = itIsOptimized;
    }

    void Logarithm::optimize()
    {
        // Simplify logarithm if the target is a power of the base
        if (_2.IsExponentiation() && _2.as<Exponentiation>().ebase() == _1) {
            Become(std::move(_2.as<Exponentiation>().eexp()));
        }
        // Simplify logarithm if base and target are the same
        else if (_1 == _2) {
            Become(1);
        } else if (_1.IsInt() && _2.IsInt() && getTarget() > constants::zero && lbase() > constants::one) {
            auto base = lbase().ca();
            auto target = getTarget().ca();
            // Use binary search for initial approximation
            auto high = target;
            decltype(high) low = 0;
            while (low < high) {
                decltype(high) mid = (low + high) / 2;
                if ((lbase() ^ mid) <= getTarget()) {
                    low = mid + 1;
                } else {
                    high = mid;
                }
            }
            decltype(high) x = low - 1;
            if ((lbase() ^ x) == getTarget()) {
                Become(std::move(x));
            }
        }
        // Add more simplification rules as needed
    }

    Valuable& Logarithm::operator +=(const Valuable& v)
    {
        // Implement addition of logarithms
        if (v.IsLogarithm() && v.as<Logarithm>().lbase() == _1) {
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
        if (v.IsLogarithm() && v.as<Logarithm>().lbase() == _1) {
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
        if (v.IsLogarithm() && v.as<Logarithm>().lbase() == _1) {
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
        if (v.IsLogarithm() && v.as<Logarithm>().lbase() == _1) {
            // If bases are the same, return true and the multiplied targets
            Valuable newTarget = _2 * v.as<Logarithm>().getTarget();
            return {true, Logarithm(_1, newTarget)};
        }
        return {false, *this};
    }

    bool Logarithm::SumIfSimplifiable(const Valuable& v)
    {
        // Check if summation can be simplified
        if (v.IsLogarithm() && v.as<Logarithm>().lbase() == _1) {
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
        if (v.IsLogarithm() && v.as<Logarithm>().lbase() == _1) {
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
        return Become(Exponentiation{*this, value});
    }

    Logarithm::operator double() const {
        LOG_AND_IMPLEMENT("double conversion for Logarithm")
    }

    Valuable& Logarithm::d(const Variable& x) {
        LOG_AND_IMPLEMENT("derivative of Logarithm with respect to " << x)
        return *this;
    }

    Valuable& Logarithm::integral(const Variable& x, const Variable& C) {
        LOG_AND_IMPLEMENT("integral of Logarithm with respect to " << x << " with constant " << C)
        return *this;
    }

}  // namespace math
}  // namespace omnn
