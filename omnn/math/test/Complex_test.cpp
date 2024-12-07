#define BOOST_TEST_MODULE Complex_test
#include <boost/test/unit_test.hpp>
#include "../Complex.h"
#include <complex>

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(complex_construction_test) {
    Complex c1(1.0, 2.0);
    BOOST_CHECK_CLOSE(c1.real(), 1.0, 1e-10);
    BOOST_CHECK_CLOSE(c1.imag(), 2.0, 1e-10);

    std::complex<double> std_c(3.0, 4.0);
    Complex c2(std_c);
    BOOST_CHECK_CLOSE(c2.real(), 3.0, 1e-10);
    BOOST_CHECK_CLOSE(c2.imag(), 4.0, 1e-10);
}

BOOST_AUTO_TEST_CASE(complex_arithmetic_test) {
    Complex c1(1.0, 2.0);
    Complex c2(3.0, 4.0);
    std::complex<double> std_c(5.0, 6.0);

    // Test Complex + Complex
    Complex sum = c1 + c2;
    BOOST_CHECK_CLOSE(sum.real(), 4.0, 1e-10);
    BOOST_CHECK_CLOSE(sum.imag(), 6.0, 1e-10);

    // Test Complex + std::complex
    Complex sum2 = c1 + std_c;
    BOOST_CHECK_CLOSE(sum2.real(), 6.0, 1e-10);
    BOOST_CHECK_CLOSE(sum2.imag(), 8.0, 1e-10);

    // Test std::complex + Complex
    Complex sum3 = std_c + c1;
    BOOST_CHECK_CLOSE(sum3.real(), 6.0, 1e-10);
    BOOST_CHECK_CLOSE(sum3.imag(), 8.0, 1e-10);

    // Test operator+=
    Complex c3(1.0, 1.0);
    c3 += c2;
    BOOST_CHECK_CLOSE(c3.real(), 4.0, 1e-10);
    BOOST_CHECK_CLOSE(c3.imag(), 5.0, 1e-10);

    c3 += std_c;
    BOOST_CHECK_CLOSE(c3.real(), 9.0, 1e-10);
    BOOST_CHECK_CLOSE(c3.imag(), 11.0, 1e-10);
}

BOOST_AUTO_TEST_CASE(complex_comparison_test) {
    Complex c1(1.0, 2.0);
    Complex c2(1.0, 2.0);
    Complex c3(1.0, 2.000000001); // Very close but different

    BOOST_CHECK(c1 == c2);
    BOOST_CHECK(!(c1 != c2));
    BOOST_CHECK(c1 == c3); // Should be equal within epsilon
}

BOOST_AUTO_TEST_CASE(complex_conversion_test) {
    Complex c1(1.0, 0.0);
    std::complex<double> std_c = c1.value();
    BOOST_CHECK_CLOSE(std_c.real(), 1.0, 1e-10);
    BOOST_CHECK_CLOSE(std_c.imag(), 0.0, 1e-10);

    Complex c2 = Complex(std_c);
    BOOST_CHECK(c1 == c2);
}

BOOST_AUTO_TEST_CASE(complex_exact_phase_test) {
    Integer num(1), den(4);  // Phase angle π/2
    Complex c = Complex::from_exact_phase(num, den);

    // Check that it stores exact phase
    BOOST_CHECK(c.is_exact_phase());

    // Verify the stored phase values
    auto [stored_num, stored_den] = c.get_exact_phase();
    BOOST_CHECK_EQUAL(stored_num, num);
    BOOST_CHECK_EQUAL(stored_den, den);

    // Check approximate values (should be close to i = 0 + 1i for π/2)
    BOOST_CHECK_SMALL(c.real(), 1e-10);
    BOOST_CHECK_CLOSE(c.imag(), 1.0, 1e-10);

    // Test phase normalization
    Complex c2 = Complex::from_exact_phase(Integer(5), Integer(4));  // 5π/2 should normalize to π/2
    auto [norm_num, norm_den] = c2.get_exact_phase();
    BOOST_CHECK_EQUAL(norm_num, Integer(1));
    BOOST_CHECK_EQUAL(norm_den, Integer(4));
}
