#define BOOST_TEST_MODULE Comparison test
#include <boost/test/unit_test.hpp>
#include "skrypt.h"

using namespace ::skrypt;
using namespace std::string_literals;
using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(Comparison_test){
    Skrypt skrypt;
    skrypt.Load(TEST_SRC_DIR "Comparison.skrypt");

    // get the variable
    auto variableName = "y"s;
    auto varhost = skrypt.GetVarHost();
    auto& pendingTheVariable = varhost->Host(variableName);
}
