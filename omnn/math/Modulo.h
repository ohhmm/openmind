/*
 * Modulo.h
 *
 *  Created on: Sep 29, 2018
 *      Author: sergejkrivonos
 */

#pragma once

#include <omnn/math/DuoValDescendant.h>
#include <omnn/math/Fraction.h>

namespace omnn::math
{

class Modulo : public DuoValDescendant<Modulo>
{
    using base = DuoValDescendant<Modulo>;
    static vars_cont_t VarsForCommoning;

public:
    using base::base;

	constexpr bool IsModulo() const override { return true; }
    auto& getDividend() const { return get1(); }
    auto& getDevisor() const { return get2(); }

    bool operator==(const Modulo&) const;

    std::ostream& print_sign(std::ostream& out) const override;
    std::ostream& code_sign(std::ostream& out) const override;
    std::ostream& code(std::ostream& out) const override;

    static constexpr auto GetBinaryOperationLambdaTemplate() {
        return [](const auto& numerator, const auto& denominator) {
            using T = std::decay_t<decltype(numerator)>;
            // Special case for Fraction type
            if constexpr (std::is_same_v<T, Fraction>) {
                // Get the values
                double num = static_cast<double>(numerator.numerator().ca());
                double den = static_cast<double>(numerator.denominator().ca());
                double mod = static_cast<double>(denominator.ca());

                // First calculate a/b
                double value = num / den;
                std::cout << "Initial fraction value: " << value << std::endl;

                // For negative modulo, we use the formula:
                // a mod n = a - n * floor(a/n)
                // This ensures the result has the same sign as n
                double quotient = value / mod;
                std::cout << "Quotient (value/mod): " << quotient << std::endl;

                double floored = std::floor(quotient);
                std::cout << "Floored quotient: " << floored << std::endl;

                double result = value - mod * floored;
                std::cout << "Result before sign adjustment: " << result << std::endl;

                // The mathematical modulo should always satisfy:
                // 1. If result is non-zero, it should have the same sign as mod
                // 2. |result| < |mod|
                if (result != 0) {
                    bool resultNegative = result < 0;
                    bool modNegative = mod < 0;
                    if (resultNegative != modNegative) {
                        result += mod;
                        std::cout << "After sign adjustment: " << result << std::endl;
                    }
                }

                std::cout << "Final result: " << result << std::endl;
                return result;
            }
            // For floating point numbers
            else if constexpr (std::is_floating_point_v<T>) {
                auto quotient = numerator / denominator;
                auto result = numerator - denominator * std::floor(quotient);

                // Ensure the result has the same sign as the denominator
                if (result != 0) {
                    bool resultNegative = result < 0;
                    bool denominatorNegative = denominator < 0;
                    if (resultNegative != denominatorNegative) {
                        result += denominator;
                    }
                }
                return result;
            }
            // For integers
            else {
                auto mod = numerator % denominator;
                // Ensure the result has the same sign as the denominator
                if (mod != 0) {
                    bool modNegative = mod < 0;
                    bool denominatorNegative = denominator < 0;
                    if (modNegative != denominatorNegative) {
                        mod += denominator;
                    }
                }
                return mod;
            }
        };
    }

    static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2);

    void optimize() override;

	Valuable operator-() const override;

    Valuable& sq() override;

	std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable& v) const override { return {}; }
    std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override { return {}; }
    Valuable InCommonWith(const Valuable& v) const override { return 1; }
    const vars_cont_t& getCommonVars() const override { return VarsForCommoning; }

	bool IsComesBefore(const Modulo&) const;
    bool IsComesBefore(const Valuable&) const override;
    omnn::math::Valuable::vars_cont_t GetVaExps() const override;
};

} /* namespace omnn::math */
