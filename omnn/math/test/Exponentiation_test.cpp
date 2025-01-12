#define BOOST_TEST_MODULE Exponentiation test
#include <boost/test/unit_test.hpp>

#include "Variable.h"
#include "Sum.h"
#include "generic.hpp"


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

    _1 = (((x-2)^2)+((x+2)^2));
    _1 = _1(x);
    _2 = ((-1)^(1_v/2))*2;
    BOOST_TEST(_1==_2);

	_1 = constants::i ^ constants::zero;
    _2 = constants::one;
    BOOST_TEST(_1==_2);
    
    _1 = constants::plus_minus_1;
    _1 *= constants::half;

    _1 = x^3;
    _2 = _1.getVaVal();
    BOOST_TEST(_1 == _2);

    _1 = Fraction{Exponentiation{x, 2}, Exponentiation{x, 1}};
    _2 = x;
    BOOST_TEST(_1 == _2);
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
    BOOST_TEST(_1 == _2);
    
    DECL_VA(x);
    _1 =(-1_v)^x;
    _2 = (-1_v)^((1_v/2)*x + _1/4 + ((-1_v)/4));
    BOOST_TEST(_1 != _2);

    _1 = 2_v/3;
    {
        Valuable::OptimizeOff off;
        _2 = Exponentiation{3, -1};
        BOOST_TEST(_1 != _2);
    }
}

BOOST_AUTO_TEST_CASE(Exponentiation_Order_test) {
    auto _1 = "((((-1)^x))/(-2))"_v;
    auto _2 = constants::half;
    InequalOrderCheck(_1, _2);
    _1 = -_1;
    _2 = -_2;
    InequalOrderCheck(_1, _2);
    _1 = "((((-1)^(x + 10)))/4)"_v;
    InequalOrderCheck(_1, _2);
    _2.sq();
    InequalOrderCheck(_1, _2);
    _1 *= 4;
    _2 = 1;
    InequalOrderCheck(_1, _2);

    _1 = "(((Y^2) + -1*sqrt((X^2)) + -2*Y + X + 1)^(-1))"_v;
    _2 = 9 ^ constants::two;
    InequalOrderCheck(_1, _2);

    _1 = Exponentiation{"X"_va, 2};
    InequalOrderCheck(_1, _2);
}

BOOST_AUTO_TEST_CASE(Exponentiation_Same_test) {
    DECL_VA(X);
    auto _1 = Exponentiation{X, 2};
    auto _2 = Exponentiation{Sum{X}, 2};

    BOOST_TEST(_1 == _2);
    auto before = _1.IsComesBefore(_2);
    auto after = _2.IsComesBefore(_1);
    Product{_1, _2};
    Sum{_1, _2};
    auto same = _1.Same(_2);
    BOOST_TEST(!same);
}

BOOST_AUTO_TEST_CASE(Exponentiation_Simplification_tests)
{
    DECL_VA(x);
    auto _1 = x*((-1*(x+100))^-1);
    _1 *= -1;
    auto _2 = x/(x+100);
    BOOST_TEST(_1==_2);

    _1 = constants::one ^ x;
    auto varless = _1.varless();
    BOOST_TEST(varless == constants::one);
}

BOOST_AUTO_TEST_CASE(Exponentiation_Deserialization_tests, *disabled()) {

    auto _1 = "sqrt(((x^2) - 16*x + 64)) ^ -1"_v;
    DECL_VA(x);
    auto _2 = (x.Sq() - 16 * x + 64).sqrt() ^ -1;
    BOOST_TEST(_1 == _2);
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

    auto _1 = constants::i;
    _1 = _1.abs();
    auto _2 = constants::one;
    BOOST_TEST(_1 == _2);

    _1 = constants::i;
    _2 = _1;
    _1.sq().sqrt();
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(Abs_test) {
    auto _1 = constants::i;
    _1.sqrt();
    _1 = _1.sqrt().abs(); // abs(sqrt(-i))
    auto _2 = constants::one;
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(Polynomial_Sqrt_test) {
    DECL_VA(x);
    DECL_VA(y);
    auto a = 4 * (x ^ 2) - 12 * x * y + 9 * (y ^ 2);
    auto e = a.Sqrt(); // As the Sqrt is principal root, it is The positive square root of a number
                    // so the Sqrt gives positive for positive value
                    // answer must depend on x,y values

    auto _1 = a;
    _1.eval({{x, 0}, {y, 1}}); // x=0, y=1
    _1.optimize();

    auto _2 = e;
    _2.eval({{x, 0}, {y, 1}}); // x=0, y=1
    _2.optimize();

    BOOST_TEST(_2 != -3);   // 2x-3y = -3
    BOOST_TEST(_2 == 3);    // 3y-2x = 3
    BOOST_TEST(_1 == _2.Sq());
    BOOST_TEST(_1.Sqrt() == _2);

    _1 = a;
    _1.eval({{x, 1}, {y, 0}}); // x=1, y=0
    _1.optimize();

    _2 = e;
    _2.eval({{x, 1}, {y, 0}}); // x=1, y=0
    _2.optimize();

    BOOST_TEST(_2 == 2); // 2 * x - 3 * y  == 2
    BOOST_TEST(_2 != -2); // 3 * y - 2 * x  == -2
    BOOST_TEST(_1 == _2.Sq());
    BOOST_TEST(_1.Sqrt() == _2);

    _1 = "sqrt(((x^2) - 16*x + 64)) ^ (-1)"_v;
    _2 = -1;
    auto is = _1.IsMultiplicationSimplifiable(_2);
    BOOST_TEST(!is.first);
}

BOOST_AUTO_TEST_CASE(Polynomial_Exp_test)
{
    DECL_VARS(a, b, v);
    
    auto _1 = (4_v*(v^2) + 2048)^(1_v/2);
    _1.SetView(Valuable::View::Solving);
    _1.optimize();
    auto _2 = (v ^ 2) + 512;
    BOOST_TEST(_1 == _2);
    

    auto t=4_v;
    auto c=2048_v;
    auto _2ab=2_v*a*b;
    auto d = (-_2ab).sq() + 4_v*(t-a.Sq())*(b.Sq()-c);
    auto x = (_2ab+(d^(1_v/2)))/(2_v*(t-a.Sq()));
    _1.eval({{v,x}});
    auto ab = 0_v;
    _1.eval({{b,ab}});
   // FIXME : BOOST_TEST(_1 == "(512 x ^ 2) / (x ^ 2 - 4)"_v);  // or x  (Valuable::View::Solving)
    

    {
    DECL_VA(x);
    _1 = ((-18_v*(x^2) + 108*x -180)^((1/3)));
    _1.SetView(Valuable::View::Equation);
    _1.optimize();
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

BOOST_AUTO_TEST_CASE(Exponentiation_InCommonWith_test) {
    DECL_VA(x);
    auto _1 = 16^x;
    auto _2 = 16^x.Sqrt();
    auto common = _1.InCommonWith(_2);
    BOOST_TEST(common == _2);

    _1 = x ^ 3;
    _2 = x ^ 2;
    common = _1.InCommonWith(_2);
    BOOST_TEST(common == _2);
}
