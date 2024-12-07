#pragma once

#include <complex>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <optional>
#include "ValuableDescendantContract.h"
#include "Valuable.h"
#include "Integer.h"
#include "Fraction.h"

namespace omnn {
namespace math {

class Complex : public ValuableDescendantContract<Complex> {
public:
    using complex_type = std::complex<double>;

    // Constructors
    Complex() : value_(0.0, 0.0) {}
    Complex(double real, double imag = 0.0) : value_(real, imag) {}
    Complex(const Complex& other) : value_(other.value_) {}
    Complex(Complex&& other) noexcept : value_(std::move(other.value_)) {}
    Complex(const complex_type& c) : value_(c) {}
    explicit Complex(const Valuable& v) {
        if (auto* c = dynamic_cast<const Complex*>(&v)) {
            value_ = c->value_;
        } else if (v.IsInt()) {
            value_ = complex_type(static_cast<double>(v.as<Integer>().as_const_base_int_ref()), 0);
        } else if (v.IsFraction()) {
            auto& f = v.as<Fraction>();
            value_ = complex_type(static_cast<double>(f.getNumerator().ca()) /
                                static_cast<double>(f.getDenominator().ca()), 0);
        } else {
            throw std::invalid_argument("Cannot convert Valuable to Complex");
        }
    }

    // Basic accessors
    double real() const { return value_.real(); }
    double imag() const { return value_.imag(); }
    double norm() const { return std::norm(value_); }

    // Assignment operators
    Complex& operator=(const Complex& other) {
        value_ = other.value_;
        return *this;
    }

    Complex& operator=(Complex&& other) noexcept {
        value_ = std::move(other.value_);
        return *this;
    }

    Complex& operator=(const complex_type& other) {
        value_ = other;
        return *this;
    }

    Complex& operator=(const Valuable& v) {
        if (auto* c = dynamic_cast<const Complex*>(&v)) {
            value_ = c->value_;
        } else if (v.IsInt()) {
            value_ = complex_type(static_cast<double>(v.as<Integer>().as_const_base_int_ref()), 0);
        } else if (v.IsFraction()) {
            auto& f = v.as<Fraction>();
            value_ = complex_type(static_cast<double>(f.getNumerator().ca()) /
                                static_cast<double>(f.getDenominator().ca()), 0);
        } else {
            throw std::invalid_argument("Cannot convert Valuable to Complex");
        }
        return *this;
    }

    // Arithmetic operators inherited from ValuableDescendantBase
    Valuable& operator+=(const Valuable& v) override {
        if (auto* c = dynamic_cast<const Complex*>(&v)) {
            value_ += c->value_;
        } else if (v.IsInt()) {
            value_ += complex_type(static_cast<double>(v.as<Integer>().as_const_base_int_ref()), 0);
        } else if (v.IsFraction()) {
            auto& f = v.as<Fraction>();
            value_ += complex_type(static_cast<double>(f.getNumerator().ca()) /
                                static_cast<double>(f.getDenominator().ca()), 0);
        } else {
            throw std::invalid_argument("Cannot convert Valuable to Complex");
        }
        return *this;
    }

    Valuable& operator*=(const Valuable& v) override {
        if (auto* c = dynamic_cast<const Complex*>(&v)) {
            value_ *= c->value_;
        } else if (v.IsInt()) {
            value_ *= complex_type(static_cast<double>(v.as<Integer>().as_const_base_int_ref()), 0);
        } else if (v.IsFraction()) {
            auto& f = v.as<Fraction>();
            value_ *= complex_type(static_cast<double>(f.getNumerator().ca()) /
                                 static_cast<double>(f.getDenominator().ca()), 0);
        }
        return *this;
    }

    Valuable& operator/=(const Valuable& v) override {
        if (auto* c = dynamic_cast<const Complex*>(&v)) {
            value_ /= c->value_;
        } else if (v.IsInt()) {
            value_ /= complex_type(static_cast<double>(v.as<Integer>().as_const_base_int_ref()), 0);
        } else if (v.IsFraction()) {
            auto& f = v.as<Fraction>();
            value_ /= complex_type(static_cast<double>(f.getNumerator().ca()) /
                                 static_cast<double>(f.getDenominator().ca()), 0);
        }
        return *this;
    }

    Valuable& operator%=(const Valuable& v) override {
        throw std::runtime_error("Modulo operation not supported for complex numbers");
    }

    Valuable& operator^=(const Valuable& v) override {
        throw std::runtime_error("Power operation not supported for complex numbers yet");
    }

    // Required by ValuableDescendantBase
    bool operator==(const Valuable& v) const override {
        if (auto* c = dynamic_cast<const Complex*>(&v)) {
            const double epsilon = 1e-10;
            return std::abs(value_.real() - c->value_.real()) < epsilon &&
                   std::abs(value_.imag() - c->value_.imag()) < epsilon;
        }
        return false;
    }

    bool IsComesBefore(const Valuable& v) const override {
        if (auto* c = dynamic_cast<const Complex*>(&v)) {
            auto mag1 = std::abs(value_);
            auto mag2 = std::abs(c->value_);
            const double epsilon = 1e-10;
            if (std::abs(mag1 - mag2) >= epsilon) return mag1 < mag2;
            return std::arg(value_) < std::arg(c->value_);
        }
        return ValuableDescendantContract<Complex>::IsComesBefore(v);
    }

    // Additional required overrides
    void optimize() override {}

    Valuable operator-() const override {
        return Complex(-value_.real(), -value_.imag());
    }

    Valuable& sq() override {
        value_ *= value_;
        return *this;
    }

    Valuable Sqrt() const override {
        return Complex(std::sqrt(value_));
    }

    bool IsSurd() const override { return false; }
    bool IsConstant() const override { return true; }
    bool IsInt() const override { return imag() == 0.0 && real() == std::floor(real()); }
    bool IsFraction() const override { return false; }
    bool IsRadical() const override { return false; }
    bool IsPrincipalSurd() const override { return false; }
    bool IsSimpleFraction() const override { return false; }
    bool IsFormula() const override { return false; }
    bool IsExponentiation() const override { return false; }
    bool IsLogarithm() const override { return false; }
    bool IsVa() const override { return false; }
    bool IsVaExp() const override { return false; }
    bool IsProduct() const override { return false; }
    bool IsSum() const override { return false; }
    bool IsInfinity() const override { return false; }
    bool IsModulo() const override { return false; }
    bool IsMInfinity() const override { return false; }
    bool IsNaN() const override { return std::isnan(real()) || std::isnan(imag()); }
    bool Is_e() const override { return false; }
    bool Is_i() const override { return real() == 0.0 && imag() == 1.0; }
    bool Is_pi() const override { return false; }
    bool IsZero() const override { return real() == 0.0 && imag() == 0.0; }
    bool IsSimple() const override { return true; }

    YesNoMaybe IsEven() const override { return IsInt() ? (static_cast<int>(real()) % 2 == 0 ? YesNoMaybe::Yes : YesNoMaybe::No) : YesNoMaybe::Maybe; }
    YesNoMaybe IsMultival() const override { return YesNoMaybe::No; }

    universal_lambda_t CompileIntoLambda(variables_for_lambda_t) const override {
        return [value = value_](const universal_lambda_params_t&) -> Valuable {
            return Complex(value);
        };
    }

    void Values(const std::function<bool(const Valuable&)>& f) const override {
        f(*this);
    }

    Valuable Univariate() const override {
        return *this;
    }

    // Conversion operators
    operator complex_type() const { return value_; }
    explicit operator double() const { return value_.real(); }

    // Binary arithmetic operators with std::complex<double>
    Complex operator-(const complex_type& other) const {
        return Complex(value_ - other);
    }

    Complex operator*(const complex_type& other) const {
        return Complex(value_ * other);
    }

    Complex operator/(const complex_type& other) const {
        return Complex(value_ / other);
    }

    // Implement operator+= for both Complex and complex_type
    Complex& operator+=(const Complex& other) {
        value_ += other.value_;
        return *this;
    }

    Complex& operator+=(const complex_type& other) {
        value_ += other;
        return *this;
    }

    // Implement operator+ through operator+= to avoid ambiguity
    Complex operator+(const Complex& other) const {
        Complex result(*this);
        result += other;
        return result;
    }

    Complex operator+(const complex_type& other) const {
        Complex result(*this);
        result += other;
        return result;
    }

    // Friend operators for reverse operations with std::complex<double>
    friend Complex operator+(const complex_type& lhs, const Complex& rhs) {
        Complex result(lhs);
        result += rhs;
        return result;
    }

    friend Complex operator-(const complex_type& lhs, const Complex& rhs) {
        return Complex(lhs - rhs.value_);
    }

    friend Complex operator*(const complex_type& lhs, const Complex& rhs) {
        return Complex(lhs * rhs.value_);
    }

    friend Complex operator/(const complex_type& lhs, const Complex& rhs) {
        return Complex(lhs / rhs.value_);
    }

    // Stream output operator
    friend std::ostream& operator<<(std::ostream& os, const Complex& c) {
        if (c.imag() == 0.0) {
            os << c.real();
        } else if (c.real() == 0.0) {
            os << c.imag() << "i";
        } else {
            os << c.real() << (c.imag() >= 0.0 ? "+" : "") << c.imag() << "i";
        }
        return os;
    }

    // Value access
    const complex_type& value() const { return value_; }
    complex_type& value() { return value_; }

    friend bool operator==(const Complex& lhs, const Complex& rhs) {
        const double epsilon = 1e-10;
        return std::abs(lhs.value_.real() - rhs.value_.real()) < epsilon &&
               std::abs(lhs.value_.imag() - rhs.value_.imag()) < epsilon;
    }

    friend bool operator!=(const Complex& lhs, const Complex& rhs) {
        return !(lhs == rhs);
    }

    // New methods for exact phase representation
    static Complex from_exact_phase(const Integer& numerator, const Integer& denominator) {
        Complex result;
        result.value_ = std::polar(1.0, 2.0 * M_PI * (numerator.to_double() / denominator.to_double()));
        result.exact_phase_ = std::make_pair(numerator % denominator, denominator);
        if (result.exact_phase_->first < 0) {
            result.exact_phase_->first += denominator;
        }
        return result;
    }

    bool is_exact_phase() const {
        return exact_phase_.has_value();
    }

    std::pair<Integer, Integer> get_exact_phase() const {
        if (!exact_phase_) {
            throw std::runtime_error("Complex number does not contain exact phase information");
        }
        return *exact_phase_;
    }

private:
    complex_type value_;
    std::optional<std::pair<Integer, Integer>> exact_phase_;  // numerator/denominator for exact phases
};

// Free functions for std::complex compatibility
inline double norm(const Complex& c) {
    return std::norm(c.value());
}

inline double abs(const Complex& c) {
    return std::abs(c.value());
}

}  // namespace math
}  // namespace omnn
