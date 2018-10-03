//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
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
		Valuable& operator /=(const Valuable& v) override;
		Valuable& operator %=(const Valuable& v) override;
		Valuable& operator^=(const Valuable&) override;
		bool operator <(const Valuable& v) const override;
//		bool operator ==(const Valuable& v) const override;
		void optimize() override;
		Valuable sqrt() const override;
		const vars_cont_t& getCommonVars() const override;
		bool IsComesBefore(const Valuable& v) const override;
		bool IsFraction() const override { return true; }
		bool IsSimpleFraction() const override { return IsSimple(); }

		using base::base;

		Fraction() : base(0, 1) {}

		Fraction(int n) : base(n, 1) {}

		Fraction(const Valuable& n)
		: base(n, 1)
		{
		}

		Fraction(const Integer& n);

        Fraction(const boost::multiprecision::cpp_dec_float_100& f);

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
        static a_int getMaxVaExp(const Valuable& _1, const Valuable& _2) {
            return _1.getMaxVaExp();
        }
        
		Fraction Reciprocal() const;
		bool IsSimple() const;
        Valuable& sq() override;
	};

}}
