#define BOOST_TEST_MODULE Sum test
#include <boost/test/unit_test.hpp>
#include "Sum.h"
#include "Variable.h"

using namespace omnn::extrapolator;

BOOST_AUTO_TEST_CASE(Sum_tests)
{
    Variable v,v1,v2;
    BOOST_TEST((-2_v/3*v1 + v1*v1 + 1_v/9) == (1_v/9 + v1*v1 + -2_v/3*v1));
    
    Valuable
        f = 1_v / 2,
        f1 = 5_v / 6,
        a = 1_v + 2 + f + 3,
        b = 6_v + f1;
    a -= b;
    auto s=a;
    s+=v;
    std::cout << s;
    BOOST_TEST(s == a+v);
    s = s*s;
    s.optimize();
    BOOST_TEST(s == 1_v/9 - (2_v/3) * v + v*v);

    BOOST_TEST((v1*v2 + v1*v1*v2 + 1/3*125*v1 + 1/3*125*v1*v1) == (v1*v2 + 1/3*125*v1 + 1/3*125*v1*v1 + v1*v1*v2));
    
    s = Sum(v+1, v1+v2);
    s.optimize();
    auto sc = Sum::cast(s);
    BOOST_TEST(sc);
    BOOST_TEST(sc->size()==4);
}

BOOST_AUTO_TEST_CASE(Become_tests)
{
    Variable v;
    Valuable f = 1_v/2;
    auto s = f+v;
    
    BOOST_TEST(reinterpret_cast<Valuable*>(&s)->exp);
    bool isValuableType = typeid(s)==typeid(Valuable);
    BOOST_TEST(isValuableType);
    bool isInnerSum = typeid(*s.exp.get())==typeid(Sum);
    BOOST_TEST(isInnerSum);
}
