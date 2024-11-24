#define BOOST_TEST_MODULE Modulo_Product_Optimization_test
#include <boost/test/unit_test.hpp>
#include <iostream>

#include <omnn/math/Modulo.h>
#include <omnn/math/Product.h>
#include <omnn/math/Variable.h>
#include <omnn/math/Integer.h>
#include "generic.hpp"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(Modulo_Product_Optimization_test)
{
    const bool prev_opt = Valuable::optimizations;
    Valuable::optimizations = false;  // Disable optimizations initially

    try {
        // Test case 1: Variable and constant
        {
            DECL_VA(v1);
            Valuable two(2);
            Valuable five(5);

            Valuable initial_prod = Product{two, v1};
            Valuable expr = Modulo(initial_prod, five);

            std::cout << "Test 1 - Before optimization: " << expr << std::endl;
            Valuable::optimizations = true;
            expr.optimize();
            std::cout << "Test 1 - After optimization: " << expr << std::endl;

            // Verify the structure matches (a mod c * b mod c) mod c
            BOOST_TEST(expr.IsModulo());
            const auto& outer_mod = expr.as<Modulo>();
            BOOST_TEST(outer_mod.get2() == five);  // Check outer modulo divisor

            // Check that the inner expression is a product
            const auto& inner = outer_mod.get1();  // Use get1() to get first operand
            std::cout << "Inner expression: " << inner << std::endl;

            // First verify it's a product
            BOOST_TEST(inner.IsProduct());

            // Then check the product terms
            const auto& inner_prod = inner.as<Product>();
            const auto& terms = inner_prod.GetConstCont();
            BOOST_TEST(terms.size() == 2);

            // Both terms should be modulos with divisor five
            for (const auto& term : terms) {
                BOOST_TEST(term.IsModulo());
                const auto& term_mod = term.as<Modulo>();
                BOOST_TEST(term_mod.get2() == five);
            }
        }

        // Test case 2: Two variables
        {
            DECL_VA(x);
            DECL_VA(y);
            Valuable m(7);

            Valuable initial_prod = Product{y, x};
            Valuable expr = Modulo(initial_prod, m);

            std::cout << "\nTest 2 - Before optimization: " << expr << std::endl;
            Valuable::optimizations = true;
            expr.optimize();
            std::cout << "Test 2 - After optimization: " << expr << std::endl;

            // Verify the structure matches (a mod c * b mod c) mod c
            BOOST_TEST(expr.IsModulo());
            const auto& outer_mod = expr.as<Modulo>();
            BOOST_TEST(outer_mod.get2() == m);  // Check outer modulo divisor

            // Check that the inner expression is a product
            const auto& inner = outer_mod.get1();  // Use get1() to get first operand
            std::cout << "Inner expression: " << inner << std::endl;

            // First verify it's a product
            BOOST_TEST(inner.IsProduct());

            // Then check the product terms
            const auto& inner_prod = inner.as<Product>();
            const auto& terms = inner_prod.GetConstCont();
            BOOST_TEST(terms.size() == 2);

            // Both terms should be modulos with divisor m
            for (const auto& term : terms) {
                BOOST_TEST(term.IsModulo());
                const auto& term_mod = term.as<Modulo>();
                BOOST_TEST(term_mod.get2() == m);
            }
        }
    } catch (...) {
        Valuable::optimizations = prev_opt;
        throw;
    }

    Valuable::optimizations = prev_opt;
}
