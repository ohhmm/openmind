#define BOOST_TEST_MODULE skryptExpOrder test
#include <boost/test/unit_test.hpp>
#include "skrypt.h"

using namespace ::skrypt;
using namespace std::string_literals;
using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(ExpOrderSkryptLoadingTest) {
    Skrypt(TEST_SRC_DIR "TestExpOrderWithBrackets.skrypt");
    Skrypt(TEST_SRC_DIR "TestExpBracketless.skrypt");
}

BOOST_AUTO_TEST_CASE(skryptExpOrdertest
    , *boost::unit_test::disabled()
) {
    Skrypt skryptWithBrackets;
    skryptWithBrackets.Load(TEST_SRC_DIR "TestExpOrderWithBrackets.skrypt");

    // get the variable
    auto variableName = "x"s;
    auto varhost = skryptWithBrackets.GetVarHost();
    auto& v = varhost->Host(variableName);

    // check that the variable is already solved
    auto solutions = skryptWithBrackets.Known(v);
    BOOST_TEST(solutions.size() == 1);
    for (auto& solution : solutions) {
        BOOST_TEST(solution == 1);
    }


    Skrypt skryptWithExpNoBrackets;
    skryptWithExpNoBrackets.Load(TEST_SRC_DIR "TestExpBracketless.skrypt");
    auto solutionsWithExpNoBrackets = skryptWithExpNoBrackets.Known(skryptWithExpNoBrackets.GetVarHost()->Host(variableName));
    BOOST_TEST(solutionsWithExpNoBrackets.size() == 1);
    for (auto& solution : solutionsWithExpNoBrackets) {
        BOOST_TEST(solution == 1);
    }
    BOOST_TEST(solutionsWithExpNoBrackets == solutions);
}

BOOST_AUTO_TEST_CASE(TestEqualityPrecedence) {
    Skrypt skryptWithBrackets;
    skryptWithBrackets.Load(TEST_SRC_DIR "TestEqualityPrecedence.skrypt");
}
