//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once

#include <omnn/math/ValuableDescendantContract.h>

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/polymorphic_text_oarchive.hpp>
#include <boost/archive/polymorphic_text_iarchive.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace omnn::math {

    class Fraction;
    class Product;
    class Sum;

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

    Integer(const Integer&) noexcept = default;
    Integer(Integer&&) noexcept = default;
    Integer& operator=(const Integer&) noexcept = default;
    Integer& operator=(Integer&&) noexcept = default;

    template<typename IntT>
    Integer(IntT&& i = 0) noexcept
    : arbitrary(std::forward<IntT>(i))
    {
        hash = std::hash<base_int>()(arbitrary);
        optimized = true;
    }

    explicit
    Integer(const std::string& s) noexcept
        : base(), arbitrary(s)
    {
        hash = std::hash<base_int>()(arbitrary);
        optimized = true;
    }

    explicit
    Integer(const std::string_view& s) noexcept
        : base(), arbitrary(s)
    {
        hash = std::hash<base_int>()(arbitrary);
        optimized = true;
    }

    explicit Integer(const Fraction& f);

    const_base_int_ref as_const_base_int_ref() const noexcept {
        return arbitrary;
    }

    Valuable FirstFactor() const override;
    bool IsPositivePowerOf2() const noexcept;
    YesNoMaybe IsRational() const noexcept override { return YesNoMaybe::Yes; }
    explicit operator int64_t() const;

    [[nodiscard]]
    bool IsInt() const noexcept override { return true; }
    [[nodiscard]]
    bool IsZero() const noexcept override { return arbitrary.is_zero(); }
    [[nodiscard]]
    bool IsSimple() const noexcept override { return true; }
    //bool IsConstant() const override { return true; }   The Integer object may be applied an arithmetic operation and this object value changed. Only Constant class objects should return IsConstant true.
    [[nodiscard]]
    bool is_optimized() const noexcept override { return true; }
    void optimize() override { MarkAsOptimized(); }
    YesNoMaybe IsEven() const override;
    [[nodiscard]]
    bool IsPolynomial(const Variable&) const noexcept override { return true; }
    [[nodiscard]]
    size_t FillPolynomialCoefficients(std::vector<Valuable>& coefficients, const Variable& v) const override {
        if (coefficients.empty())
            coefficients.resize(1);
        coefficients[0] += *this;
        return 0;
    }
    [[nodiscard]]
    YesNoMaybe IsMultival() const noexcept override { return YesNoMaybe::No; }
    [[nodiscard]]
    const PrincipalSurd* PrincipalSurdFactor() const noexcept override { return {}; }
    void Values(const std::function<bool(const Valuable&)>& f) const override { f(*this); }

    // virtual operators
    Valuable operator -() const override;
    Valuable& operator --() override;
    Valuable& operator ++() override;
    Valuable& operator +=(int) override;
    Valuable& operator +=(const Valuable&) override;
    Valuable& operator *=(const Valuable&) override;
    Valuable& operator ^=(const Valuable&) override;
    Valuable& operator /=(const Valuable&) override;
    Valuable& operator %=(const Valuable&) override;
    [[nodiscard]]
    a_int Complexity() const override { return 1; }
    bool MultiplyIfSimplifiable(const Valuable&) override;
    std::pair<bool,Valuable> IsMultiplicationSimplifiable(const Valuable&) const override;
    bool SumIfSimplifiable(const Valuable&) override;
    std::pair<bool,Valuable> IsSummationSimplifiable(const Valuable&) const override;
    std::pair<bool, Valuable> IsModSimplifiable(const Valuable&) const override;

    vars_cont_t GetVaExps() const override { return {}; }
    std::pair<Valuable,Valuable> GreatestCommonExp(const Valuable& e) const; // exp,result
    Valuable& d(const Variable& x) override;

    Valuable Sign() const override;
    bool operator <(const Valuable& v) const override;
    friend bool operator<(const Integer& _1, const Integer& _2) { return _1.arbitrary < _2.arbitrary; }
    friend bool operator<=(const Integer& _1, const Integer& _2) { return _1.arbitrary <= _2.arbitrary; }
    friend bool operator<(const Integer& _1, int _2) { return _1.arbitrary < _2; }
    bool operator ==(const Valuable&) const override;
    bool operator ==(const Integer&) const;
    bool operator ==(const a_int&) const;
    bool operator ==(const int&) const;

    explicit operator int() const override;
    explicit operator a_int() const override;
    [[nodiscard]] constexpr explicit operator const_base_int_ref() { return arbitrary; }
    explicit operator uint64_t() const override;
    explicit operator double() const override;
    explicit operator long double() const override;
    explicit operator a_rational() const override;
    explicit operator uint32_t() const override;
    explicit operator unsigned char() const override;
    a_int& a() override;
    const a_int& ca() const override;

    Valuable bit(const Valuable& n) const override;
    Valuable Or(const Valuable& n, const Valuable& v) const override;
    Valuable And(const Valuable& n, const Valuable& v) const override;
    Valuable Xor(const Valuable& n, const Valuable& v) const override;
    Valuable Not(const Valuable& n) const override;
    Valuable& shl() override;
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

    bool eval(const std::map<Variable, Valuable>& with) override { return {}; }
    void Eval(const Variable& va, const Valuable& v) override { }
    Valuable calcFreeMember() const override;
    const vars_cont_t& getCommonVars() const override { return emptyCommonVars(); }
    Valuable& sq() override { return *this *= *this; }
    Valuable Sqrt() const override;
    bool IsComesBefore(const Product&) const;
    bool IsComesBefore(const Sum&) const;
    bool IsComesBefore(const Valuable&) const override;
    Valuable InCommonWith(const Valuable& v) const override;
    Valuable GCD(const Valuable& v) const override;
    Valuable LCM(const Valuable& v) const override;
    Valuable& lcm(const Valuable& v) override;

    std::wstring save(const std::wstring&) const override;
    std::ostream& code(std::ostream&) const override;
    universal_lambda_t CompileIntoLambda(variables_for_lambda_t) const override;

    static const ranges_t empty_zero_zone;

    void solve(const Variable&, solutions_t&) const override {}
    Valuable::solutions_t GetIntegerSolution(const Variable& va) const override;

    /// <summary>
    /// Enumerate Integer factors
    /// </summary>
    /// <param name="f">functor factors visitor</param>
    /// <param name="max">upper gauge limit for factor to check</param>
    /// <param name="zz">specific value ranges for factors to check</param>
    /// <returns>true for the first found value for which f functor param returned true</returns>
    bool Factorization(const std::function<bool(const Valuable&)>& f,
                       const Valuable& max,
                       const ranges_t& zz = empty_zero_zone) const;
    bool IsPrime() const;
    std::deque<Valuable> Divisors() const;
    std::set<Valuable> FactSet() const;
    bool SimpleFactorization(const std::function<bool(const Valuable&)>& f, const Valuable& max,
                       const ranges_t& zz = empty_zero_zone) const;
    std::set<Valuable> SimpleFactsSet() const;
    Valuable& factorial() override;
    Valuable& reciprocal() override;

    solutions_t Distinct() const final { return { arbitrary }; }
    Valuable Univariate() const final { return *this; }

    /// <summary>
    /// 0 or 1
    /// </summary>
    /// <returns>0->0, otherwise 1</returns>
    Valuable BoolIntModNotZero() const override { return ca() == 0 ? 0 : 1; }

    /// <summary>
    /// IntMod prefix tells that this method is only applicable for variables known to be integer with modulo operation
    /// applicable
    /// x%(x-1) is 1 for x>2, is 0 for 2
    /// </summary>
    /// <returns>For x meant to be integers, returns an expression that is equal to 0 when x is positive</returns>
    Valuable IntMod_IsPositive() const override { return ca() > 0 ? 0 : 1; }

    /// <summary>
    /// (this < 0) - the int is negative
    /// </summary>
    /// <returns>bool</returns>
    Valuable IntMod_Negative() const override { return ca() < 0 ? 0 : 1; }

    /// <summary>
    /// Getting sign of the assumed integer
    /// </summary>
    /// <param name="a"></param>
    /// <returns>-1, 0, 1</returns>
    Valuable IntMod_Sign() const override { return ca() < 0 ? -1 : (ca() > 0 ? 1 : 0); }

    /// <summary>
    /// Converts the operator to boolean
    /// </summary>
    /// <returns>An expression that evaluates to 1 or 0 value</returns>
    Valuable ToBool() const override { return ca() == 0 ? 1 : 0; }

    /// <summary>
    /// (x-1)%x is -1 for negative numbers only
    /// (x-1)%x is undefined for zero
    /// (x-1)%x is x-1 for positive integers
    /// </summary>
    /// <returns></returns>
    Valuable IntMod_IsNegativeOrZero() const override { return ca() <= 0 ? 0 : 1; }
    Valuable NegativeOrZero() const override { return IntMod_IsNegativeOrZero(); }

    /// <summary>
    /// Operator 'less' then value to which a param expression is to be evaluated
    /// IntMod prefix tells that this method is only applicable for variables known to be integer
    /// Modulo operation may be used
    /// </summary>
    /// <param name="than">the param to compare that the object is less then the param</param>
    /// <returns>An expression that equals zero only when the object is less then param</returns>
    Valuable IntMod_Less(const Valuable& than) const override {
        if (than.IsInt())
            return ca() < than.ca() ? 0 : 1;
        else if (than.IsSimpleFraction())
            return than > *this ? 1 : 0;
        else
			return base::IntMod_Less(than);
	}

    Valuable MustBeInt() const override { return 0; }

private:
	base_int arbitrary = 0;

    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& archive, const unsigned int version) {
        archive & boost::serialization::base_object<Valuable>(*this);
        archive & BOOST_SERIALIZATION_NVP(arbitrary);
    }
};

}
