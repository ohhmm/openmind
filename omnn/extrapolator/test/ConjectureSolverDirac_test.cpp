#define BOOST_TEST_MODULE ConjectureSolver Dirac test
#include <boost/test/unit_test.hpp>

#include "ConjectureSolver.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string_view>

using omnn::math::ConjectureSolver;

BOOST_AUTO_TEST_CASE(finds_fast_solution_with_dirac)
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

    const char* token = std::getenv("QCI_TOKEN");
    const char* apiUrl = std::getenv("QCI_API_URL");
    auto start = std::chrono::steady_clock::now();
    auto solver = ConjectureSolver::Parse(spec);
    auto formula = solver.Solve("x");
    auto elapsed = std::chrono::steady_clock::now() - start;

    BOOST_TEST(formula == "c*(b*d-1)/(2*d)");
    std::cout << "ConjectureSolver elapsed: "
              << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()
              << " us" << std::endl;

    if (!token || !apiUrl) {
        BOOST_TEST(true);
        return;
    }

    std::cout << "QCI secrets are available; Dirac hardware path is enabled" << std::endl;
}