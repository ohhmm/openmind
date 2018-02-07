//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include <type_traits>
#include "ValuableDescendantContract.h"

namespace omnn{
namespace math {
    
    class Fraction;

class Integer
    : public ValuableDescendantContract<Integer>
{
    using base = ValuableDescendantContract<Integer>;

protected:
    std::ostream& print(std::ostream& out) const override;

public:
    using base_int = a_int;

	using const_base_int_ref = const base_int&;
	
	using base::base;

    Integer(const Integer&)=default;
    Integer(Integer&&)=default;
	Integer& operator=(const Integer& f) = default;

    template<class IntT>
    static Integer From(typename std::enable_if<std::is_integral<IntT>::value>::type i = 0)
    {
        return Integer(boost::multiprecision::cpp_int(i));
    }

    Integer(int i = 0)
        : arbitrary(i)
    {
        hash = std::hash<base_int>()(arbitrary);
    }

    Integer(const long i)
    : arbitrary(i)
    {
        hash = std::hash<base_int>()(arbitrary);
    }
    
    Integer(unsigned long i)
    : arbitrary(i)
    {
        hash = std::hash<base_int>()(arbitrary);
    }
    
    Integer(unsigned long long i)
    : arbitrary(i)
    {
        hash = std::hash<base_int>()(arbitrary);
    }
    
    Integer(const base_int& i)
        : arbitrary(i)
    {
        hash = std::hash<base_int>()(arbitrary);
    }
    
    explicit Integer(const Fraction& f);
    
    operator const_base_int_ref() const {
        return arbitrary;
    }
    
    explicit operator int64_t() const;

    bool IsInt() const override { return true; }
    
    // virtual operators 
    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator +=(int v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override;
    Valuable& operator ++() override;
    Valuable& operator^=(const Valuable&) override;
    Valuable& d(const Variable& x) override;
    bool operator <(const Valuable& v) const override;
    bool operator ==(const Valuable& v) const override;
    // virtual convert operators
    explicit operator int() const override;
    explicit operator size_t() const override;
    explicit operator double() const override;
    explicit operator long double() const override;
    explicit operator unsigned char() const override;

    const Variable* FindVa() const override { return nullptr; }
    bool HasVa(const Variable& va) const override { return false; }
    void CollectVa(std::set<Variable>&) const override { }
    void Eval(const Variable& va, const Valuable& v) override { }
    Valuable calcFreeMember() const override;
    const vars_cont_t& getCommonVars() const override { return emptyCommonVars(); }
    Valuable sqrt() const override { return *this^(1_v/2); }
    
    bool Factorization(const std::function<bool(const Integer&)>& f) const;

private:
	base_int arbitrary = 0;
};


}
}

