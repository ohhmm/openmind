//
// Created by Сергей Кривонос on 01.09.17.
//
#include "omnn/math/Exponentiation.h"
#include "omnn/math/Fraction.h"
#include "omnn/math/Modulo.h"
#include "omnn/math/PrincipalSurd.h"
#include "omnn/math/Product.h"
#include "omnn/math/Sum.h"

namespace omnn{
namespace math {

Valuable& ValuableDescendantBase::operator^=(const Valuable& v)
{
    return Become(Exponentiation(*this,v));
	// FIXME: TODO: return Become(Exponentiation(std::move(*this), v));
}

Valuable& ValuableDescendantBase::operator+=(const Valuable &v)
{
    return Become(Sum{*this,v});
}

Valuable& ValuableDescendantBase::operator*=(const Valuable &v)
{
    return Become(Product{*this,v});
}

Valuable& ValuableDescendantBase::operator/=(const Valuable &v)
{
    return Become(Fraction(*this,v));
}

Valuable& ValuableDescendantBase::operator%=(const Valuable &v)
{
    return Become(Modulo(*this,v));
}

Valuable ValuableDescendantBase::operator -() const
{
    return *this * -1;
}

void ValuableDescendantBase::Values(const std::function<bool(const Valuable&)> &fun) const {
    auto imv = IsMultival();
    if (imv == YesNoMaybe::No)
        fun(*this);
    else
        LOG_AND_IMPLEMENT(*this << " Values method");
}

Valuable ValuableDescendantBase::Univariate() const {
	auto uni = constants::one;
	for (auto &branch : Distinct()) {
		uni.logic_or(branch);
	}
	return uni;
}

Valuable ValuableDescendantBase::Sqrt() const {
    return PrincipalSurd{*this};
}

bool ValuableDescendantBase::IsComesBefore(const Valuable& v) const {
    return *this > v;
}

Valuable::universal_lambda_t ValuableDescendantBase::CompileIntoLambda(variables_for_lambda_t) const {
    LOG_AND_IMPLEMENT("Implement CompileIntoLambda: " << *this);
}

} // namespace math
} // namespace omnn
