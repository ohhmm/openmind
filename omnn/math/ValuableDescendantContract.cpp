//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Exponentiation.h"
#include "Fraction.h"
#include "Modulo.h"
#include "PrincipalSurd.h"
#include "Product.h"
#include "Sum.h"

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

Valuable& ValuableDescendantBase::sq() {
    return Become(Exponentiation{*this, 2});
}

Valuable ValuableDescendantBase::Sqrt() const {
    return PrincipalSurd{*this};
}

bool ValuableDescendantBase::IsComesBefore(const Valuable& v) const {
    LOG_AND_IMPLEMENT("Implement " << *this << " IsComesBefore " << v);
    return *this > v;
}

Valuable::universal_lambda_t ValuableDescendantBase::CompileIntoLambda(variables_for_lambda_t) const {
    LOG_AND_IMPLEMENT("Implement CompileIntoLambda: " << *this);
}

} // namespace math
} // namespace omnn
