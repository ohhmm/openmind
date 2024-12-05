#define BOOST_TEST_MODULE Differ test
#include <boost/test/unit_test.hpp>

#include "FormulaOfVaWithSingleIntegerRoot.h"
#include "Variable.h"
#include "Sum.h"
#include "generic.hpp"


BOOST_AUTO_TEST_CASE(Different_Value_test) {
    DECL_VARS(Idx1, Idx2, Value1, Value2);
    auto IndexesAreDifferent = Idx1.NotEquals(Idx2);
    auto ValuesAreDifferent = Value1.NotEquals(Value2);
    auto Different = IndexesAreDifferent && ValuesAreDifferent;
    std::cout << "Equation of non-repeating sequences: " << Different << std::endl;
}
