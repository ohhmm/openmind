//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include "ValuableDescendantContract.h"

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
    const Valuable& getBase() const { return ebase; }
    const Valuable& getExponentiation() const { return eexp; }
    /// returns r/w accessor
    Valuable getBase();
    /// returns r/w accessor
    Valuable getExponentiation();
    
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
    {
        hash = ebase.Hash() ^ eexp.Hash();
    }

    Exponentiation(const Valuable& b)
            : ebase(b), eexp(1)
    {
        hash = ebase.Hash() ^ eexp.Hash();
    }

    const Variable* FindVa() const override;
    void CollectVa(std::set<Variable>& s) const override;
    void Eval(const Variable& va, const Valuable& v) override;
};

}}
