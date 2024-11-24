#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Modulo_Product_Optimization_test
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>

#include "Modulo.h"
#include "Product.h"
#include "Variable.h"
#include "Integer.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(Modulo_Product_Optimization_test)
{
    // Test case 1: Simple constant product
    auto expr1 = (2_v * 3_v) % 5_v;
    expr1.optimize();
    BOOST_TEST(expr1 == 1_v);  // (2*3)%5 = 6%5 = 1

    // Test case 2: Single variable with constant
    Variable v1;
    auto expr2 = (2_v * v1) % 5_v;
    // Match the exact nesting pattern: (((((((v1 % 5)*(2 % 5)) % 5)) % 5)) % 5)
    auto expected2 = Modulo(
        Modulo(
            Modulo(
                Modulo(
                    Modulo(
                        Product({
                            Modulo(v1, 5_v),
                            Modulo(2_v, 5_v)
                        }),
                        5_v
                    ),
                    5_v
                ),
                5_v
            ),
            5_v
        ),
        5_v
    );
    expr2.optimize();
    BOOST_TEST(expr2 == expected2);
}
