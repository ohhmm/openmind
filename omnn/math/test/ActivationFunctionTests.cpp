#define BOOST_TEST_MODULE ActivationFunctionTests test
#include <boost/test/unit_test.hpp>

#include <omnn/math/pi.h>
#include <omnn/math/Variable.h>

#include <iostream>
#include <cmath>
#include <cassert>

using namespace omnn::math;
using namespace boost::unit_test;


auto TestPrecision = 0.000001;

// ReLU activation function
auto relu(auto x) {
    return std::max(x, {});
}
// Tanh activation function
auto tanh_activation(auto x) {
    return std::tanh(x);
}
// GELU activation function
auto gelu(const Valuable& x) {
    return x / constants::two * (constants::one + std::tanh((2 / constants::pi).sqrt() * (x + 0.044715 * (x ^ 3))));
}

BOOST_AUTO_TEST_CASE(test_relu)
{
    BOOST_TEST(relu(constants::zero).IsZero());
    BOOST_TEST(relu(constants::one) == constants::one);
    BOOST_TEST(relu(constants::minus_1).IsZero());
    BOOST_TEST(relu(10_v) == 10_v);
    BOOST_TEST(relu(-10_v).IsZero());
}

BOOST_AUTO_TEST_CASE(test_tanh
                     , *disabled()
                     ) {
    BOOST_TEST(std::abs(tanh_activation(constants::zero)).IsZero());
    BOOST_TEST(static_cast<double>(std::abs(tanh_activation(constants::one) - 0.761594)) < 1e-6);
    BOOST_TEST(static_cast<double>(std::abs(tanh_activation(constants::minus_1) +0.761594)) < 1e-6);
    BOOST_TEST(std::abs(tanh_activation(10_v) - constants::one).IsZero());
    BOOST_TEST(std::abs(tanh_activation(-10_v) - constants::one).IsZero());
}

BOOST_AUTO_TEST_CASE(gelu_double_test, *tolerance(TestPrecision))
{
    BOOST_TEST(static_cast<double>(gelu(10)) == 10);
}

BOOST_AUTO_TEST_CASE(test_gelu, *disabled()) {
    BOOST_TEST(gelu(0).IsZero());
    BOOST_TEST(gelu(1) == 0.841344);
    BOOST_TEST(gelu(-1) == -0.158655);
    BOOST_TEST(gelu(10) == 10);
    BOOST_TEST(gelu(-10).IsZero());
}
