#define BOOST_TEST_MODULE ConjectureSolver Dirac test
#include <boost/test/unit_test.hpp>

#include "ConjectureSolver.h"
#include <omnn/quantum/QuantumAccelerationFacility.h>
#include <omnn/quantum/ProblemTranslator.h>
#include <omnn/math/Valuable.h>
#include <omnn/math/Variable.h>

#include <chrono>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <string_view>

using omnn::math::ConjectureSolver;
using omnn::quantum::QuantumAccelerationFacility;

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

    QuantumAccelerationFacility facility;
    auto objective = std::make_shared<omnn::math::Valuable>("x");
    facility.SetObjective(objective);

    auto diracStart = std::chrono::steady_clock::now();
    const bool ok = facility.Invoke();
    auto diracElapsed = std::chrono::steady_clock::now() - diracStart;
    BOOST_TEST(ok);
    auto solution = facility.GetOptimalSolution();
    BOOST_TEST(!solution.empty());
    std::cout << "Dirac elapsed: "
              << std::chrono::duration_cast<std::chrono::microseconds>(diracElapsed).count()
              << " us, solution size: " << solution.size() << std::endl;
}
