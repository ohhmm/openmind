#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <cmath>
#endif

#define BOOST_TEST_MODULE ModularExponentiation test
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include "../ModularExponentiation.h"
#include "../QuantumRegister.h"
#include "../Integer.h"

using namespace omnn::math;

std::vector<int> continued_fraction(double x, int max_terms = 10, double epsilon = 1e-10) {
    std::vector<int> terms;
    double remainder = x;

    for (int i = 0; i < max_terms && std::abs(remainder) > epsilon; ++i) {
        int term = static_cast<int>(std::floor(remainder));
        terms.push_back(term);
        remainder = 1.0 / (remainder - term);

        if (!std::isfinite(remainder)) break;
    }
    return terms;
}

std::pair<int, int> evaluate_convergent(const std::vector<int>& terms, size_t n) {
    if (n >= terms.size()) n = terms.size() - 1;
    if (n == 0) return {terms[0], 1};

    int num = terms[n], den = 1;
    for (int i = n - 1; i >= 0; --i) {
        int temp = num;
        num = terms[i] * num + (i == n - 1 ? 1 : den);
        den = temp;
    }
    return {num, den};
}

BOOST_AUTO_TEST_CASE(ModularExponentiation_basic_test) {
    QuantumRegister reg(6);

    reg.hadamard(0);
    reg.phase(0, Integer(1), Integer(2));
    reg.hadamard(0);

    ModularExponentiation::apply(reg, Integer(2), Integer(15), 0, 2, 2, 6);

    auto result = reg.measure_range(2, 6);
    BOOST_TEST(result == Integer(2));
}

BOOST_AUTO_TEST_CASE(ModularExponentiation_error_handling_test) {
    QuantumRegister reg(4);

    BOOST_CHECK_THROW(
        ModularExponentiation::apply(reg, Integer(2), Integer(15), 0, 5, 2, 4),
        std::invalid_argument
    );

    BOOST_CHECK_THROW(
        ModularExponentiation::apply(reg, Integer(15), Integer(15), 0, 2, 2, 4),
        std::invalid_argument
    );
}

BOOST_AUTO_TEST_CASE(ModularExponentiation_superposition_test) {
    QuantumRegister reg(6);

    reg.hadamard(0);

    ModularExponentiation::apply(reg, Integer(7), Integer(15), 0, 2, 2, 6);

    const auto& state = reg.get_state();
    double total_prob = 0.0;
    for (const auto& amp : state) {
        total_prob += std::abs(amp.value()) * std::abs(amp.value());
    }
    BOOST_TEST(std::abs(total_prob - 1.0) < 1e-10);
}

BOOST_AUTO_TEST_CASE(ModularExponentiation_period_test) {
    QuantumRegister reg(8);

    for (size_t i = 0; i < 3; ++i) {
        reg.hadamard(i);
    }

    ModularExponentiation::apply(reg, Integer(2), Integer(21), 0, 3, 3, 8);

    auto result = reg.measure_range(3, 8);

    std::stringstream ss;
    ss << result;
    double result_val = std::stod(ss.str());
    double measured_phase = result_val / (1LL << 5);

    measured_phase -= std::floor(measured_phase);

    auto fractions = continued_fraction(measured_phase, 6, 1e-6);

    double best_error = std::numeric_limits<double>::max();
    double best_period = 0;

    for (size_t i = 0; i < fractions.size(); ++i) {
        auto convergent = evaluate_convergent(fractions, i);
        double period = convergent.second;

        if (period > 0 && period <= 6) {
            Integer base(2);
            Integer test = base.Power(Integer(period)) % Integer(21);
            if (test == Integer(1)) {
                double error = std::abs(period - 6.0);
                if (error < best_error) {
                    best_error = error;
                    best_period = period;
                }
            }
        }
    }

    BOOST_TEST(std::abs(best_period - 6.0) < 1.0);
}
