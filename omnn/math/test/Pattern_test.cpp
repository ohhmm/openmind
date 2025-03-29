#define BOOST_TEST_MODULE pattern
#include <boost/test/unit_test.hpp>

#include <omnn/math/Pattern.h>
#include <omnn/math/Valuable.h>
#include <omnn/math/Variable.h>
#include <omnn/math/VarHost.h>
#include <omnn/math/Product.h>

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(pattern_creation_test) BOOST_TEST_DISABLED()
{
    auto host = VarHost::make<std::string>();
    
    Variable x("x", host);
    Valuable expr = x * x;
    Pattern f("f", {x}, expr);
    
    BOOST_TEST(f.GetName() == "f");
    BOOST_TEST(f.GetArguments().size() == 1);
    BOOST_TEST(f.GetExpression() == expr);
    
    Variable y("y", host);
    Valuable expr2 = x * y + x;
    Pattern g("g", {x, y}, expr2);
    
    BOOST_TEST(g.GetName() == "g");
    BOOST_TEST(g.GetArguments().size() == 2);
    BOOST_TEST(g.GetExpression() == expr2);
}

BOOST_AUTO_TEST_CASE(pattern_string_constructor_test) BOOST_TEST_DISABLED()
{
    auto host = VarHost::make<std::string>();
    
    Pattern f("Function(f, [x], x^2)", host);
    
    BOOST_TEST(f.GetName() == "f");
    BOOST_TEST(f.GetArguments().size() == 1);
    
    Pattern g("Function(g, [x, y], x*y+x)", host);
    
    BOOST_TEST(g.GetName() == "g");
    BOOST_TEST(g.GetArguments().size() == 2);
}

BOOST_AUTO_TEST_CASE(pattern_application_test) BOOST_TEST_DISABLED()
{
    auto host = VarHost::make<std::string>();
    
    Variable x("x", host);
    Valuable expr = x * x;
    Pattern f("f", {x}, expr);
    
    Valuable two(2);
    Valuable result = f({two});
    BOOST_TEST(result == 4);
    
    Variable y("y", host);
    Valuable expr2 = x * y + x;
    Pattern g("g", {x, y}, expr2);
    
    Valuable three(3);
    Valuable result2 = g({two, three});
    BOOST_TEST(result2 == 9); // 2 * 3 + 2 = 8
}

BOOST_AUTO_TEST_CASE(pattern_equality_test) BOOST_TEST_DISABLED()
{
    auto host = VarHost::make<std::string>();
    
    Variable x("x", host);
    Valuable expr = x * x;
    Pattern f1("f", {x}, expr);
    Pattern f2("f", {x}, expr);
    
    BOOST_TEST(f1 == f2);
    
    Pattern g("g", {x}, expr);
    BOOST_TEST(f1 != g);
}
