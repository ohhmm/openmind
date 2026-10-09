#define BOOST_TEST_MODULE skryptissue10 test
#include <boost/test/unit_test.hpp>
#include "skrypt.h"

using namespace ::skrypt;
using namespace std::string_literals;
using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(skryptissue10test){
    Skrypt skrypt;
    skrypt.Load(TEST_SRC_DIR "TestIssue10.skrypt");

    // get the variable
    auto variableName = "pending"s;
    auto varhost = skrypt.GetVarHost();
    auto& pendingTheVariable = varhost->Host(variableName);

    // check that the variable is already solved
    auto& pendingVariableSolutions = skrypt.Known(pendingTheVariable);
    BOOST_TEST(pendingVariableSolutions.size() == 1);
    for (auto& solution : pendingVariableSolutions) {
        BOOST_TEST(solution == 10);
    }
}
