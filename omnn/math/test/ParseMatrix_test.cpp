#define BOOST_TEST_MODULE ParseMatrix
#include "Variable.h"
#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

using namespace omnn::math;

static void check(const char* s, const Valuable& expected) {
    Valuable v(s, expected.getVaHost(), false);
    if (v != expected) {
        std::cout << "MISMATCH: \"" << s << "\" -> " << v << " (expected " << expected << ")" << std::endl;
    }
    BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_CASE(parse_matrix) {
    DECL_VA(x);
    // binary vs unary minus
    check("-5", -5_v);
    check("5-3", 5_v - 3_v);
    check("5 - 3", 5_v - 3_v);
    check("-5 + 3", -5_v + 3_v);
    check("5 * -3", 5_v * -3_v);
    check("5 * - (2 + 1)", 5_v * -(2_v + 1_v));
    check("5 / -2", 5_v / -2_v);
    check("2 ^ -1", 2_v ^ -1_v);
    check("(1 - 2) * -3", (1_v - 2_v) * -3_v);
    // decimals
    check("1.5", 1.5_v);
    check("-1.25", -1.25_v);
    check("2 * -1.5", 2_v * -1.5_v);
    // functions variables
    check("sqrt(4)", (4_v).Sqrt());
    check("x^2 - 16*x + 64", x.Sq() - 16*x + 64);
}
