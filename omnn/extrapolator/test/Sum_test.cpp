#define BOOST_TEST_MODULE Sum test
#include <boost/test/unit_test.hpp>

#include "Sum.h"
#include "Fraction.h"
#include "Variable.h"


using namespace omnn::extrapolator;

BOOST_AUTO_TEST_CASE(Sum_tests)
{
    using s_t = Sum;
    using f_t = Fraction;
    Variable v;
    f_t f(1 , 2);
    f_t f1(5, 6);
    s_t a(1,2, f, 3) ;
    s_t b(6, f1);
    a -= b;
    auto s=a;
    s+=v;
    std::cout << s;
    
    BOOST_TEST(s == a+v);
}

BOOST_AUTO_TEST_CASE(Become_tests)
{
    Variable v;
    Fraction f = 1/2;
    auto s = f+v;
    
    BOOST_TEST(reinterpret_cast<Valuable*>(&s)->exp);
    bool isInnerSum = typeid(s.exp)==typeid(Sum);
    BOOST_TEST(isInnerSum);
    bool isValuableType = typeid(s)==typeid(Valuable);
    BOOST_TEST(isValuableType);
}
