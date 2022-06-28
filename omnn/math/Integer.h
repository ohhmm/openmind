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
    std::wostream& print(std::wostream& out) const override;
    bool IsSubObject(const Valuable& o) const override { return this == &o.get(); }

public:
    using base_int = a_int;

	using const_base_int_ref = const base_int&;
	
	using base::base;

    MSVC_CONSTEXPR Integer(const Integer&)=default;
    MSVC_CONSTEXPR Integer(Integer&&)=default;
    MSVC_CONSTEXPR Integer& operator=(const Integer&) = default;
    MSVC_CONSTEXPR Integer& operator=(Integer&&) = default;

    template<class IntT>
    static MSVC_CONSTEXPR Integer From(typename std::enable_if<std::is_integral<IntT>::value>::type i = 0)
    {
        return Integer(boost::multiprecision::cpp_int(i));
    }

    constexpr
    Integer(int i = 0)
        : arbitrary(i)
    {
        hash = std::hash<base_int>()(arbitrary);
    }

    template<class IntT>
    constexpr
    Integer(typename std::enable_if<std::is_integral<IntT>::value>::type i)
    : arbitrary(i)
    {
        hash = std::hash<base_int>()(arbitrary);
    }

    constexpr
    Integer(const base_int& i)
        : arbitrary(i)
    {
        hash = std::hash<base_int>()(arbitrary);
    }
    
    constexpr
    Integer(base_int&& i)
    : arbitrary(std::move(i))
    {
        hash = std::hash<base_int>()(arbitrary);
    }
    
    constexpr
    Integer(const std::string& s)
        : arbitrary(s)
    {
        hash = std::hash<base_int>()(arbitrary);
    }

    constexpr
    Integer(const std::string_view& s)
		: arbitrary(s)
    {
        hash = std::hash<base_int>()(arbitrary);
    }

    explicit Integer(const Fraction& f);
    
    const_base_int_ref as_const_base_int_ref() const {
        return arbitrary;
    }
    
    explicit operator int64_t() const;

    bool IsInt() const override { return true; }
    bool IsSimple() const override { return true; }
    //bool IsConstant() const override { return true; }   The Integer object may be applied an arithmetic operation and this object value changed. Only Constant class objects should return IsConstant true.
    bool is_optimized() const override { return true; }
    YesNoMaybe IsEven() const override;
    YesNoMaybe IsMultival() const override { return YesNoMaybe::No; }
    void Values(const std::function<bool(const Valuable&)>& f) const override { f(*this); }
    
    // virtual operators 
    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator +=(int v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator ^=(const Valuable& v) override;
    a_int Complexity() const override { return 1; }
    bool MultiplyIfSimplifiable(const Valuable& v) override;
    std::pair<bool,Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;
    bool SumIfSimplifiable(const Valuable& v) override;
    std::pair<bool,Valuable> IsSummationSimplifiable(const Valuable& v) const override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override;
    Valuable& operator ++() override;
    omnn::math::Valuable &extracted(const omnn::math::Valuable &dn, const omnn::math::Valuable &exponentiating, const omnn::math::Valuable &x);
    
    std::pair<Valuable,Valuable> GreatestCommonExp(const Valuable& e) const; // exp,result
    Valuable& d(const Variable& x) override;
    bool operator <(const Valuable& v) const override;
    friend bool operator<(const Integer& _1, const Integer& _2) { return _1.arbitrary < _2.arbitrary; }
    friend bool operator<=(const Integer& _1, const Integer& _2) { return _1.arbitrary <= _2.arbitrary; }
    friend bool operator<(const Integer& _1, int _2) { return _1.arbitrary < _2; }
    bool operator ==(const Valuable& v) const override;
    // virtual convert operators
    explicit operator int() const override;
    explicit operator a_int() const override;
    explicit operator uint64_t() const override;
    explicit operator double() const override;
    explicit operator long double() const override;
    explicit operator uint32_t() const override;
    explicit operator unsigned char() const override;
    a_int& a() override;
    const a_int& ca() const override;
    
    Valuable bit(const Valuable& n) const override;
    Valuable Or(const Valuable& n, const Valuable& v) const override;
    Valuable And(const Valuable& n, const Valuable& v) const override;
    Valuable Xor(const Valuable& n, const Valuable& v) const override;
    Valuable Not(const Valuable& n) const override;
    Valuable& shl(const Valuable& n) override;
    Valuable& shr(const Valuable& n) override;
    Valuable& shr() override { return shr(1); }
    Valuable Shr() const override;
    Valuable Shr(const Valuable& n) const override;
    
    Valuable operator()(const Variable&) const override { return 0; }
    Valuable operator()(const Variable&, const Valuable& augmentation) const override { return 0; }
    
    const Variable* FindVa() const override { return nullptr; }
    bool HasVa(const Variable& va) const override { return {}; }
    void CollectVa(std::set<Variable>&) const override { }
    void CollectVaNames(Valuable::va_names_t& s) const override { }
    Valuable varless() const override { return *this; }
    
    bool eval(const std::map<Variable, Valuable>& with) override { return {}; }
    void Eval(const Variable& va, const Valuable& v) override { }
    Valuable calcFreeMember() const override;
    const vars_cont_t& getCommonVars() const override { return emptyCommonVars(); }
    Valuable Sqrt() const override;
    bool IsComesBefore(const Valuable& v) const override;
    Valuable InCommonWith(const Valuable& v) const override;

    std::wstring save(const std::wstring&) const override;
    std::ostream& code(std::ostream&) const override;
    
    static const ranges_t empty_zero_zone;
    bool Factorization(const std::function<bool(const Valuable&)>& f,
                       const Valuable& max,
                       const ranges_t& zz = empty_zero_zone) const;
    bool IsPrime() const;
    std::deque<Valuable> Facts() const;
    std::set<Valuable> FactSet() const;
    bool SimpleFactorization(const std::function<bool(const Valuable&)>& f, const Valuable& max,
                       const ranges_t& zz = empty_zero_zone) const;
    std::set<Valuable> SimpleFactsSet() const;
    void factorial() override;

    solutions_t Distinct() const final { return { *this }; }
    Valuable Univariate() const final { return *this; }
private:
	base_int arbitrary = 0;
};


}
}

