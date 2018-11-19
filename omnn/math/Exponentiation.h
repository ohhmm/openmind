//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include <omnn/math/Variable.h>
#include <omnn/math/DuoValDescendant.h>

namespace omnn{
namespace math {

class Exponentiation
        : public DuoValDescendant<Exponentiation>
{
	using base = DuoValDescendant<Exponentiation>;
    vars_cont_t v;
protected:
    std::ostream& print_sign(std::ostream& out) const override;
    std::ostream& code(std::ostream& out) const override;
public:
    bool IsExponentiation() const override { return true; }
    const Valuable& ebase() const { return _1; }
    const Valuable& eexp() const { return _2; }
    Valuable& ebase() { return base::_1; }
    Valuable& eexp() { return base::_2; }
    const Valuable& getBase() const { return _1; }
    const Valuable& getExponentiation() const { return _2; }

    static max_exp_t getMaxVaExp(const Valuable& b, const Valuable& e);
    max_exp_t getMaxVaExp()  const override;

    // virtual operators
    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator ^=(const Valuable&) override;
    explicit operator double() const override;
    Valuable& d(const Variable& x) override;
    Valuable I(const Variable& x, const Variable& C) const override;
    Valuable& i(const Variable& x, const Variable& C) override;
    
    bool operator <(const Valuable& v) const override;
    void optimize() override;
    
    using base::base;

    const vars_cont_t& getCommonVars() const override;
    bool IsComesBefore(const Valuable& v) const override;
    Valuable calcFreeMember() const override;

    Valuable& sq() override;

    Valuable operator()(const Variable& v, const Valuable& augmentation) const override;
};

}}
