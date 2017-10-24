//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include "Integer.h"

namespace omnn{
namespace extrapolator {

class Fraction
        : public ValuableDescendantContract<Fraction>
{
	using base = ValuableDescendantContract<Fraction>;
	using base_int = omnn::extrapolator::Valuable;
    using const_base_int_ref = const base_int&;
	base_int numerator, denominator;
protected:
    std::ostream& print(std::ostream& out) const override;
public:
    // virtual operators
    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator +=(int v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override;
    Valuable& operator ++() override;
    bool operator <(const Valuable& v) const override;
    bool operator ==(const Valuable& v) const override;
    void optimize() override;
    size_t Hash() const override;
    
    using base::base;
    
	Fraction() : numerator(0), denominator(1)
	{ }
	
    Fraction(int n)
            : numerator(n), denominator(1)
    {}
    
    Fraction(const_base_int_ref n)
    : numerator(n)
    , denominator(1)
    {
		auto e = cast(n);
		if (e)
		{
			numerator = e->numerator;
			denominator = e->denominator;
		}
	}
	Fraction(const Integer& n);
    
    Fraction(const_base_int_ref n, const_base_int_ref d)
            : numerator(n), denominator(d)
    {}
    
    Fraction(Fraction&&) = default;
    Fraction(const Fraction&)=default;
    Fraction& operator=(const Fraction& f)=default;
    Fraction& operator=(Fraction&& f)=default;
	//Fraction& operator=(const Fraction&) = default;

    const base_int& getDenominator() const;
    const base_int& getNumerator() const;

    Fraction Reciprocal() const;
    const Variable* FindVa() const override;
    void Eval(const Variable& va, const Valuable& v) override;
};

}}
