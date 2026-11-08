//
// Created by Sergej Krivonos on 07.11.26.
//
#define BOOST_TEST_MODULE ConjectureSolver test
#include <boost/test/unit_test.hpp>

#include "ConjectureSolver.h"

using omnn::math::ConjectureSolver;

BOOST_AUTO_TEST_CASE(issue_2322_solver)
{
    constexpr std::string_view spec = R"({
   a=6
   b=23
   d=5
   [
      {
         c=25
         d=5
         x=285
      }
      {
         c=50
         x=570
      }
    ]
})";
    auto solver = ConjectureSolver::Parse(spec);
    auto formula = solver.Solve("x");
    BOOST_TEST(formula == "c*(b*d-1)/(2*d)");

    // a is constant across the samples and is not required by the simplest exact formula.
    auto at = [](std::int64_t a, std::int64_t b, std::int64_t d, std::int64_t c) {
        (void)a;
        return c * (b * d - 1) / (2 * d);
    };
    BOOST_TEST(at(6, 23, 5, 25) == 285);
    BOOST_TEST(at(6, 23, 5, 50) == 570);
}

BOOST_AUTO_TEST_CASE(direct_proportion)
{
    ConjectureSolver solver;
    solver.Add({{"x", 2}, {"y", 4}});
    solver.Add({{"x", 3}, {"y", 6}});
    BOOST_TEST(solver.Solve("y") == "2*x");
}

BOOST_AUTO_TEST_CASE(constant_and_equality)
{
    ConjectureSolver constants;
    constants.Add({{"y", 5}});
    constants.Add({{"y", 5}});
    BOOST_TEST(constants.Solve("y") == "5");

    ConjectureSolver same;
    same.Add({{"a", 4}, {"x", 4}});
    same.Add({{"a", 7}, {"x", 7}});
    BOOST_TEST(same.Solve("x") == "a");
}
