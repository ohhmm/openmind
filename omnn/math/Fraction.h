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
		Valuable& numerator() { return _1; }
        Valuable& denominator() { return _2; }
		std::ostream& print_sign(std::ostream& out) const override;

	public:
        std::ostream& code(std::ostream& out) const override;

        const Valuable& numerator() const { return _1; }
        template<class T>
        void setNumerator(T&& n)
        { set1(std::forward<T>(n)); }
        void updateNumerator(std::function<void(decltype(_1)&)>& f) { update1(f); }

        const Valuable& denominator() const { return _2; }
		template<class T>
        void setDenominator(T&& n)
        { set2(std::forward<T>(n)); }
        void updateDenominator(std::function<void(decltype(_2)&)>& f) { update2(f); }

        Valuable operator -() const override;
		Valuable& operator +=(const Valuable& v) override;
		Valuable& operator *=(const Valuable& v) override;
        bool MultiplyIfSimplifiable(const Valuable& v) override;
        std::pair<bool,Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;
        bool SumIfSimplifiable(const Valuable& v) override;
        std::pair<bool,Valuable> IsSummationSimplifiable(const Valuable& v) const override;
		Valuable& operator /=(const Valuable& v) override;
		Valuable& operator %=(const Valuable& v) override;
		Valuable& operator^=(const Valuable&) override;
        Valuable& d(const Variable& x) override;
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
        YesNoMaybe IsMultival() const override { return _1.IsMultival() || _2.IsMultival(); }

		using base::base;

		Fraction() : base(0, 1) {}

		Fraction(int n) : base(n, 1) {}
        
        Fraction(double d)
        : Fraction(boost::multiprecision::cpp_dec_float_100(d))
        { }
        
		Fraction(const Valuable& n)
		: base(n, 1_v)
		{
		}

        explicit
		Fraction(const Integer&);
		Fraction(boost::rational<a_int>&&);
        Fraction(a_rational&&);
        Fraction(const a_rational&);
        Fraction(const boost::multiprecision::cpp_dec_float_100&);

		Fraction(Fraction&&) = default;
		Fraction(const Fraction&)=default;
		Fraction& operator=(const Fraction& f)=default;
		Fraction& operator=(Fraction&& f)=default;

        const Valuable& getDenominator() const { return _2; }
        const Valuable& getNumerator() const { return _1; }

        explicit operator unsigned char() const override;
        explicit operator a_int() const override;
        explicit operator a_rational() const override;
        operator boost::multiprecision::cpp_dec_float_100() const;
        Valuable operator()(const Variable&, const Valuable& augmentation) const override;
        static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2) {
            return _1.getMaxVaExp();
        }
        
        Fraction Reciprocal() const;
        Valuable& sq() override;
        Valuable Sign() const override;
        Valuable abs() const override;
    };

}}
