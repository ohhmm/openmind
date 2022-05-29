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
    _1 = -1_v*(-4_v^constants::half)*(-16^constants::half);
    _2 = constants::plus_minus_1 * 8 * constants::i;
    BOOST_TEST(_1==_2);
    auto d1 = _1.Distinct();
    
    _1 = (((x-2)^2)+((x+2)^2));
    _1 = _1(x);
    _2 = ((-1)^(1_v/2))*2;
    BOOST_TEST(_1==_2);

	_1 = constants::i ^ constants::zero;
    _2 = constants::one;
    BOOST_TEST(_1==_2);
    
    _1 = constants::plus_minus_1;
    _1 *= constants::half;
}

BOOST_AUTO_TEST_CASE(Compare_test)
{
    auto _1 = 25 ^ Fraction{1_v,-2};
    auto _2 = (1_v / 5)*(1_v^(1_v/2));
    if (_1.IsProduct())
        for(auto&a:_1.as<Product>())
            std::cout << a.Hash() << std::endl;
    if (_2.IsProduct())
        for(auto&a:_2.as<Product>())
            std::cout << a.Hash() << std::endl;
    auto c = _1 == _2;
    BOOST_TEST(c);
    
    DECL_VA(x);
    _1 =(-1_v)^x;
    _2 = (-1_v)^((1_v/2)*x + _1/4 + ((-1_v)/4));
    c = _1 != _2;
    BOOST_TEST(c);
}

BOOST_AUTO_TEST_CASE(Exponentiation_Simplification_tests)
{
    DECL_VA(x);
    auto _1 = x*((-1*(x+100))^-1);
    _1 *= -1;
    auto _2 = x/(x+100);
    BOOST_TEST(_1==_2);
}

BOOST_AUTO_TEST_CASE(Sqrt_test)
{
    auto a = 25_v;
    auto e = a ^ (1_v/2);
    auto c = e == (1_v^(1_v/2))*5;
    BOOST_TEST(c);
    BOOST_TEST(e != 5);
    // https://math.stackexchange.com/questions/41784/convert-any-number-to-positive-how/41787#comment5776496_41787
    e = a.Sqrt(); // Check that the middle term is two times the product of the numbers being squared in the first term and third term.
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

BOOST_AUTO_TEST_CASE(Polynomial_Exp_test)
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
    
    auto& ebs = _.as<Exponentiation>().getBase().as<Sum>();
    for(auto& m: ebs){
//        if (m.) {
//            <#statements#>
//        }
    }
    auto it = ebs.begin();
    auto m1 = *it;
    for(auto& v: it->getCommonVars())
        std::cout << v.first.str() << '^' << v.second.str() << std::endl;
    auto m2 =*++it;
    auto ms = m1+m2;
//    Valuable f = Fraction{
    
//    auto av = _(a);
    _.optimize();

    {
    DECL_VA(x);
    auto _ = ((-18_v*(x^2) + 108*x -180)^((1/3)));
    _.SetView(Valuable::View::Equation);
    _.optimize();
    }
}

BOOST_AUTO_TEST_CASE(Multival_test)
{
    auto a = 1_v ^ (1_v/2);
    BOOST_TEST(!!a.IsMultival() == true);

	a = a / a;
    BOOST_TEST(!!a.IsMultival() == true);

	// TODO : check with mathematicians which way is correct, fix the test and enable it
    //DECL_VA(x);
    //BOOST_TEST(!!x.IsMultival() == true);
    //a = x / x;
    
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
    //BOOST_TEST(a != 1);
    //
    //
    //BOOST_TEST(!!a.IsMultival() == true);
    //BOOST_TEST(a == x/x);
}
