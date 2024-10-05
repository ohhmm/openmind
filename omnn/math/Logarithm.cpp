#include "Logarithm.h"

#include "Exponentiation.h"
#include "Fraction.h"

#include <cmath>

namespace omnn {
namespace math {

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

    }

    Valuable& Logarithm::operator +=(const Valuable& v)
    {
	IMPLEMENT
	return base::operator +=(v);
    }

    Valuable& Logarithm::operator *=(const Valuable& v)
    {
	IMPLEMENT
	return base::operator *=(v);
    }

    bool Logarithm::MultiplyIfSimplifiable(const Valuable& v)
    {
        IMPLEMENT
	return base::MultiplyIfSimplifiable(v);
    }

    std::pair<bool, Valuable> Logarithm::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        IMPLEMENT
        return base::IsMultiplicationSimplifiable(v);
    }

    bool Logarithm::SumIfSimplifiable(const Valuable& v)
    {
        IMPLEMENT
        return base::SumIfSimplifiable(v);
    }

    std::pair<bool, Valuable> Logarithm::IsSummationSimplifiable(const Valuable& v) const {
        IMPLEMENT
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
