#define BOOST_TEST_MODULE DevinLogicInterface_test
#include <boost/test/unit_test.hpp>
#include <omnn/logic/DevinLogicInterface.h>

using namespace omnn::logic;
using namespace omnn::math;

BOOST_AUTO_TEST_SUITE(DevinLogicInterface_test)

BOOST_AUTO_TEST_CASE(test_variable_creation) {
    auto var = DevinLogicInterface::createVariable("x");
    BOOST_CHECK(var.IsVa());
}

BOOST_AUTO_TEST_CASE(test_logical_operations) {
    auto x = DevinLogicInterface::createVariable("x");
    auto y = DevinLogicInterface::createVariable("y");
    
    auto andExpr = DevinLogicInterface::logicalAnd(x, y);
    auto orExpr = DevinLogicInterface::logicalOr(x, y);
    
    BOOST_CHECK(andExpr.Is<Valuable>());
    BOOST_CHECK(orExpr.Is<Valuable>());
}

BOOST_AUTO_TEST_CASE(test_equality) {
    auto x = DevinLogicInterface::createVariable("x");
    auto y = DevinLogicInterface::createVariable("y");
    
    auto eqExpr = DevinLogicInterface::equals(x, y);
    BOOST_CHECK(eqExpr.Is<Valuable>());
}

BOOST_AUTO_TEST_SUITE_END()
