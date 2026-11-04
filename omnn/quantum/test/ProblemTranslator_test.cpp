#define BOOST_TEST_MODULE ProblemTranslator test
#include <omnn/math/Valuable.h>
#include <omnn/quantum/ProblemTranslator.h>

#include <boost/test/unit_test.hpp>

#include <string>

BOOST_AUTO_TEST_CASE (translates_polynomial) {
    omnn::math::Valuable x("x");
    omnn::math::Valuable objective = x;

    auto json = omnn::quantum::ProblemTranslator::Translate(objective);
    BOOST_TEST(json.find("\"vars\": [\"x\"]") != std::string::npos);
    BOOST_TEST(json.find("\"coeff\":") != std::string::npos);
}
