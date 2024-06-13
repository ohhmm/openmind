#define BOOST_TEST_MODULE TestRelu test
#include <boost/test/unit_test.hpp>

#include <omnn/math/pi.h>
#include <omnn/math/Variable.h>

#include <iostream>
#include <cmath>
#include <cassert>

using namespace omnn::math;
using namespace boost::unit_test;


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

// Test function for GELU
BOOST_AUTO_TEST_CASE(test_gelu
                     , *disabled()
                     )
{
    BOOST_TEST(gelu(0).IsZero());
    // FIXME: compilability
    // BOOST_TEST(std::abs(gelu(1) - 0.841344) < (constants::one ^ -6));
    // BOOST_TEST(std::abs(gelu(-1) - (-0.158655)) < (constants::one ^ -6));
    BOOST_TEST(gelu(10) == 10);
    BOOST_TEST(gelu(-10).IsZero());
}

BOOST_AUTO_TEST_CASE(test_activation_empty) {
}