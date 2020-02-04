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
        const Valuable& numerator() const { return _1; }
        Valuable& denominator() { return _2; }
		const Valuable& denominator() const { return _2; }
		std::ostream& print_sign(std::ostream& out) const override;

	public:
		Valuable operator -() const override;
		Valuable& operator +=(const Valuable& v) override;
		Valuable& operator *=(const Valuable& v) override;
        bool MultiplyIfSimplifiable(const Valuable& v) override;
        std::pair<bool,Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;
        bool SumIfSimplifiable(const Valuable& v) override;
        std::pair<bool,Valuable> IsSumationSimplifiable(const Valuable& v) const override;
		Valuable& operator /=(const Valuable& v) override;
		Valuable& operator %=(const Valuable& v) override;
		Valuable& operator^=(const Valuable&) override;
        Valuable& d(const Variable& x) override;
        bool operator ==(const Valuable& v) const override;
		bool operator <(const Valuable& v) const override;
        explicit operator double() const override;
        
		void optimize() override;
		Valuable Sqrt() const override;
		const vars_cont_t& getCommonVars() const override;
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
		: base(n, 1)
		{
		}

		Fraction(const Integer&);
		Fraction(boost::rational<a_int>&&);
		Fraction(const boost::multiprecision::cpp_dec_float_100&);

		Fraction(Fraction&&) = default;
		Fraction(const Fraction&)=default;
		Fraction& operator=(const Fraction& f)=default;
		Fraction& operator=(Fraction&& f)=default;

        const Valuable& getDenominator() const { return _2; }
        const Valuable& getNumerator() const { return _1; }

        explicit operator unsigned char() const override;
        explicit operator a_int() const override;
        operator boost::multiprecision::cpp_dec_float_100() const;
        Valuable operator()(const Variable&, const Valuable& augmentation) const override;
        static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2) {
            return _1.getMaxVaExp();
        }
        
		Fraction Reciprocal() const;
		bool IsSimple() const;
        Valuable& sq() override;
	};

}}
