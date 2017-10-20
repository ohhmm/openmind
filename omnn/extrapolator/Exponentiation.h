//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include "Integer.h"

namespace omnn{
namespace extrapolator {

class Exponentiation
        : public ValuableDescendantContract<Exponentiation>
{
	using base = ValuableDescendantContract<Exponentiation>;
	Valuable ebase, eexp;
protected:
    std::ostream& print(std::ostream& out) const override;
public:
    // virtual operators
    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator --() override;
    Valuable& operator ++() override;
    bool operator <(const Valuable& v) const override;
    bool operator ==(const Valuable& v) const override;
    void optimize() override;
    
    using base::base;
    
    Exponentiation(const Valuable& b, const Valuable& e)
            : ebase(b), eexp(e)
    {}

    Exponentiation(const Valuable& b)
            : ebase(b), eexp(1)
    {}
};

}}
