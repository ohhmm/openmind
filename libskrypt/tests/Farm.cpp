#define BOOST_TEST_MODULE Farm test
#include <boost/test/unit_test.hpp>
#include "skrypt.h"

using namespace ::skrypt;
using namespace std::string_literals;
using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(ChemistryMinimalTest) {
    Skrypt skrypt;
    skrypt.Load(TEST_SRC_DIR "Farm.skrypt");

    // get the variable
    auto variableName = "Ducks"s;
    auto varhost = skrypt.GetVarHost();
    auto& pendingTheVariable = varhost->Host(variableName);

    // check that the variable is already solved
    auto& nitrogenValencySolutions = skrypt.Known(pendingTheVariable);
    BOOST_TEST(nitrogenValencySolutions.size() == 1);
    for (auto& solution : nitrogenValencySolutions) {
        //BOOST_TEST(solution == );
    }
}
