#define BOOST_TEST_MODULE modules test
#include "skrypt.h"
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;
using namespace ::skrypt;
using namespace std::string_literals;
using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(LoadingModulesNoExceptionsTest, *disabled()) {
    Skrypt skrypt(TEST_SRC_DIR "Module.skrypt");
    auto copy = skrypt;
}

BOOST_AUTO_TEST_CASE(ModulePropertyTest, *disabled()) {
    Skrypt skrypt;
    skrypt.AddModuleSearchDirPath(TEST_SRC_DIR);
    skrypt.Load("Module.skrypt");

    // get the variable
    auto variableName = "x"s;
    auto varhost = skrypt.GetVarHost();

    auto& variable = varhost->Host(variableName);
    auto& solutions = skrypt.Known(variable);
    BOOST_TEST(solutions.size() == 1);
    for (auto& solution : solutions) {
        BOOST_TEST(solution == 1);
    }

    auto& moduleVariableX = varhost->Host("TestExpOrderWithBrackets.x"s);
    auto& moduleVariableSolutions = skrypt.Known(moduleVariableX);

    // check that the variable is already solved
    BOOST_TEST(moduleVariableSolutions.size() == 1);
    for (auto& solution : moduleVariableSolutions) {
        BOOST_TEST(solution == 1);
    }
}

BOOST_AUTO_TEST_CASE(ModuleNoTest) {
}
