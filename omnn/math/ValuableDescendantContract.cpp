//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Exponentiation.h"
#include "Fraction.h"
#include "Modulo.h"
#include "Product.h"
#include "Sum.h"

namespace omnn{
namespace math {

Valuable& ValuableDescendantBase::operator^=(const Valuable& v)
{
    return Become(Exponentiation(*this,v));
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
	auto uni = 1_v;
	for (auto &branch : Distinct()) {
		uni.logic_or(branch);
	}
	return uni;
}

}}
