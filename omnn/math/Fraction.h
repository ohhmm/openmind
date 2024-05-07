//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include <boost/multiprecision/detail/number_base.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <omnn/math/Variable.h>
#include <omnn/math/DuoValDescendant.h>

namespace omnn{
namespace math {

    class Integer;

    class Fraction
            : public DuoValDescendant<Fraction>
    {
        using base = DuoValDescendant<Fraction>;

        mutable vars_cont_t vars;

    protected:
        constexpr Valuable& numerator() { return _1; }
        constexpr Valuable& denominator() { return _2; }
        std::ostream& print_sign(std::ostream& out) const override;

    public:
        std::ostream& code(std::ostream& out) const override;

        constexpr const Valuable& numerator() const { return _1; }
        template<class T>
        void setNumerator(T&& n)
        { set1(std::forward<T>(n)); }
        void updateNumerator(std::function<void(decltype(_1)&)>& f) { update1(f); }

        constexpr const Valuable& denominator() const { return _2; }
        template<class T>
        void setDenominator(T&& n)
        { set2(std::forward<T>(n)); }
        void updateDenominator(std::function<void(decltype(_2)&)>& f) { update2(f); }

        Valuable operator -() const override;
        Valuable& operator +=(const Valuable& v) override;
        Valuable& operator *=(const Valuable& v) override;
        Valuable& operator *=(const Fraction& v);
        bool MultiplyIfSimplifiable(const Valuable& v) override;
        std::pair<bool,Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;
        bool SumIfSimplifiable(const Valuable& v) override;
        std::pair<bool,Valuable> IsSummationSimplifiable(const Valuable& v) const override;
        Valuable& operator /=(const Valuable& v) override;
        Valuable& operator %=(const Valuable& v) override;
        Valuable& operator^=(const Valuable&) override;
        Valuable& d(const Variable& x) override;
        bool operator==(const Fraction&) const;
        bool operator ==(const Valuable& v) const override;
        bool operator <(const Valuable& v) const override;
        explicit operator double() const override;
        void solve(const Variable& va, solutions_t&) const override;
        Valuable::solutions_t Distinct() const override;

        void optimize() override;
        Valuable Sqrt() const override;
        const vars_cont_t& getCommonVars() const override;
        vars_cont_t GetVaExps() const override;
        Valuable InCommonWith(const Valuable& v) const override;
        bool IsComesBefore(const Valuable& v) const override;
        bool IsFraction() const override { return true; }
        bool IsSimpleFraction() const override { return IsSimple(); }
        bool IsZero() const override { return _1.IsZero() && !_2.IsZero() && !_2.IsInfinity(); }
        YesNoMaybe IsMultival() const override { return _1.IsMultival() || _2.IsMultival(); }
        bool IsPolynomial(const Variable& x) const override {
            return _1.IsPolynomial(x) && _2.IsSimple();
        }

        using base::base;

        explicit Fraction(double d)
        : Fraction(boost::multiprecision::cpp_dec_float_100(d))
        { }

        Fraction(const Valuable& n)
        : base(n, constants::one)
        {
        }

        explicit
        Fraction(const Integer&);
        Fraction(const boost::multiprecision::cpp_dec_float_100&);

        Fraction(const boost::multiprecision::cpp_rational& r)
        : base(::boost::multiprecision::numerator(r), boost::multiprecision::denominator(r))
        {}

        Fraction(const boost::rational<a_int>& r)
        : base(r.numerator(), r.denominator())
        {}

        Fraction()
        : Fraction(constants::zero)
        {}

        Fraction(int n)
        : Fraction(a_rational(n, 1))
        {}

        Fraction(Fraction&&) = default;
        Fraction(const Fraction&)=default;
        Fraction& operator=(const Fraction& f)=default;
        Fraction& operator=(Fraction&& f)=default;

        constexpr const Valuable& getDenominator() const { return _2; }
        constexpr const Valuable& getNumerator() const { return _1; }

        explicit operator unsigned char() const override;
        explicit operator a_int() const override;
        explicit operator a_rational() const override;
        operator boost::multiprecision::cpp_dec_float_100() const;
        Valuable operator()(const Variable&, const Valuable& augmentation) const override;
        static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2) {
            return _1.getMaxVaExp();
        }

        Valuable& reciprocal() override;
        Valuable Reciprocal() const override;
        Valuable& sq() override;
        Valuable Sign() const override;
        Valuable abs() const override;
    };

} // namespace math
} // namespace omnn
