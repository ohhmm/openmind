//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include "ValuableDescendantContract.h"

namespace omnn{
namespace math {

class Exponentiation
        : public ValuableDescendantContract<Exponentiation>
{
	using base = ValuableDescendantContract<Exponentiation>;
	Valuable ebase, eexp;
    mutable vars_cont_t v;
protected:
    std::ostream& print(std::ostream& out) const override;
    std::ostream& code(std::ostream& out) const override;
public:
    bool IsExponentiation() const override { return true; }
    const Valuable& getBase() const { return ebase; }
    const Valuable& getExponentiation() const { return eexp; }
    a_int getMaxVaExp() const override;

    // virtual operators
    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator ^=(const Valuable&) override;
    Valuable& operator --() override;
    Valuable& operator ++() override;
    Valuable& d(const Variable& x) override;
    bool operator <(const Valuable& v) const override;
    bool operator ==(const Valuable& v) const override;
    void optimize() override;
    
    using base::base;
    
    Exponentiation(const Valuable& b, const Valuable& e);

//    Exponentiation(const Valuable& b)
//            : ebase(b), eexp(1)
//    {
//        hash = ebase.Hash() ^ eexp.Hash();
//    }
//
    const Variable* FindVa() const override;
    bool HasVa(const Variable& va) const override { return ebase.HasVa(va) || eexp.HasVa(va); }
    void CollectVa(std::set<Variable>& s) const override;
    const vars_cont_t& getCommonVars() const override;
    void Eval(const Variable& va, const Valuable& v) override;
    bool IsComesBefore(const Valuable& v) const override;
    Valuable calcFreeMember() const override;

};

}}
