#define BOOST_TEST_MODULE Valuable Hamiltonian test
#include <boost/test/unit_test.hpp>

#include "omnn/math/Valuable.h"
#include "omnn/math/Variable.h"

BOOST_AUTO_TEST_CASE(hamiltonian_terms_for_simple_quadratic) {
    omnn::math::Variable x("x");
    omnn::math::Variable y("y");
    omnn::math::Valuable::va_names_t variables{{"x", x}, {"y", y}};
    omnn::math::Valuable expression(std::string("x*y + 2*x + 3"), variables);

    auto terms = expression.HamiltonianTerms();

    BOOST_REQUIRE(!terms.empty());
}
