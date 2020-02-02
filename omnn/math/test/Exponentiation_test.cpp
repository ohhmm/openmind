#define BOOST_TEST_MODULE Exponentiation test
#include <boost/test/unit_test.hpp>

#include "Variable.h"
#include "Sum.h"
using namespace omnn::math;
using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(Exponentiation_tests)
{
    auto a = 2_v ^ 3;
    BOOST_TEST(a==8);
    a = 2_v ^ 11;
    BOOST_TEST(a==(1<<11));
    Variable x;
    auto _1 = (x-2)^3;
    auto _2 = (x-2)*(x-2)*(x-2);
    BOOST_TEST(_1==_2);
    _1 = -32;
    _1 ^= .5;
    _2 = 4;
    _2 *= -2_v^(1_v/2);
    BOOST_TEST(_1==_2);
    _1 = -1_v*((-4_v)^(1_v/2))*((-16_v)^((1_v/2)));
    _2 = 8;
    BOOST_TEST(_1==_2);
    
    _1 = (((x-2)^2)+((x+2)^2));
    _1 = _1(x);
    _2 = ((-1)^(1_v/2))*2;
    BOOST_TEST(_1==_2);
}

BOOST_AUTO_TEST_CASE(Compare_test)
{
    auto _1 = 25 ^ Fraction{1_v,-2};
    auto _2 = (1_v / 5)*(1_v^(1_v/2));
    auto p =Product::cast(_1);
    if (p) for(auto&a:*p)
        std::cout << a.Hash() << std::endl;
    p =Product::cast(_2);
    if (p) for(auto&a:*p)
        std::cout << a.Hash() << std::endl;
    auto c = _1 == _2;
    BOOST_TEST(c);
}

BOOST_AUTO_TEST_CASE(Sqrt_test)
{
    auto a = 25_v;
    auto e = a ^ (1_v/2);
    auto c = e == (1_v^(1_v/2))*5;
    BOOST_TEST(c);
    BOOST_TEST(e != 5);
    // https://math.stackexchange.com/questions/41784/convert-any-number-to-positive-how/41787#comment5776496_41787
    e = a.Sqrt();
    BOOST_TEST(e == 5);
}

BOOST_AUTO_TEST_CASE(Polynomial_Sqrt_test
                     ,*disabled()
                     )
{
    Variable x,y;
    auto a = 4*(x^2)-12*x*y+9*(y^2);
    auto e = a.Sqrt();
    BOOST_TEST(e == 2*x-3*y);
}

BOOST_AUTO_TEST_CASE(Polynomial_Exp_test
                     ,*disabled()
                     )
{
    auto v = "v"_va;
    auto a = "a"_va;
    auto b = "b"_va;
    
    auto _ = (4_v*(v^2) + 2048)^(1_v/2);
    _.SetView(Valuable::View::Solving);
    _.optimize();
    
    auto t=4_v;
    auto c=2048_v;
    auto _2ab=2_v*a*b;
    auto d = (-_2ab).sq() + 4_v*(t-a.Sq())*(b.Sq()-c);
    auto x = (_2ab+(d^(1_v/2)))/(2_v*(t-a.Sq()));
    _.eval({{v,x}});
    auto ab = 0_v;
    _.eval({{b,ab}});
    
    auto e = Exponentiation::cast(_);
    auto eb = e->getBase();
    auto ebs = Sum::cast(eb);
    for(auto&m:*ebs){
//        if (m.) {
//            <#statements#>
//        }
    }
    auto it =ebs->begin();
    auto m1=*it;
    for(auto& v:it->getCommonVars())
        std::cout << v.first.str() << '^' << v.second.str() << std::endl;
    auto m2 =*++it;
    auto ms = m1+m2;
//    Valuable f = Fraction{
    
//    auto av = _(a);
    _.optimize();
}

// TODO : check with mathematicians which way is correct, fix te test and enable it
BOOST_AUTO_TEST_CASE(Multival_test
                    ,*disabled()
                    )
{
    auto a = 1_v ^ (1_v/2);
    BOOST_TEST(!!a.IsMultival() == true);
    
    DECL_VA(x);
    BOOST_TEST(!!x.IsMultival() == true);
    a = x / x;
    
    // Waht is the value of a?
    // first impression is that x/x is allways 1
    // what about multivalue expressions like this?
    // it equals both values by disjunction: either -1 or 1 at the same time
    // this would be a circle function on image
    // the division of multiples we could apply in an alternative way:
    // permutations of the operation on possible values:
    // [-1/1, 1/1, 1/-1, -1/-1]  => [1, -1] != 1
    // so this way the answer should remain multival 1^(1/2)
    // 1^(1/2) / 1^(1/2) == 1^(1/2)
    // TODO : check with mathematicians which way is correct, fix te test and enable it
    BOOST_TEST(a != 1);
    
    
    BOOST_TEST(!!a.IsMultival() == true);
    BOOST_TEST(a == x/x);
}
