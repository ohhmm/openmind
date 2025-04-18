#define BOOST_TEST_MODULE Sum test
#include <boost/test/unit_test.hpp>
#include "Sum.h"
#include "Infinity.h"
#include "Variable.h"
#include "generic.hpp"


using namespace omnn::math;
using namespace boost::unit_test;
using namespace std::string_view_literals;


template<class T>
std::string l(const T& v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

void ohashes(const Valuable& v)
{
    for(auto& i : v.as<Sum>())
    {
        std::cout << i << " hash "<< i.Hash()<<std::endl;
    }
}

BOOST_AUTO_TEST_CASE(SumBalancingTest) {
    DECL_VARS(x);
    auto _1 = 2*x + constants::plus_minus_1 - 3;
    _1.SetView(Valuable::View::Solving);
    _1.optimize();

    _1 = 2 * x + (x ^ 2);
    _1.SetView(Valuable::View::Solving);
    _1.optimize();
    auto _ = _1.Solutions();
    BOOST_TEST(_.size() == 2);
}

BOOST_AUTO_TEST_CASE(SumSurdReduce_test) {
    auto equation = "-4 + sqrt(-1*(x^2) + 8)"_v;
    auto reduced = equation.Optimized(Valuable::View::SupersetOfRoots);
    auto& variable = *equation.FindVa();
    BOOST_TEST(reduced == "(x^2) + 8"_v);
    auto roots = reduced.solve(variable);
    BOOST_TEST(roots.size() == 2);
    roots = equation.solve(variable);
    BOOST_TEST(roots.size() == 2);
}

BOOST_AUTO_TEST_CASE(SumMultivalTestRootTest
    , *disabled()
) {
    DECL_VARS(a);
    auto _1 = 2 * a + constants::plus_minus_1 - 3;
    auto isRoot = _1.Test(a, -1);
    BOOST_TEST(isRoot);
    isRoot = _1.Test(a, 2);
    BOOST_TEST(isRoot);
    auto roots1 = _1.solve(a);
    auto ok = roots1.size() == 2;
    BOOST_TEST(ok);

    auto _2 = _1;
    _2.SetView(Valuable::View::Equation);
    _2.optimize();
    auto roots2 = _2.solve(a);
    BOOST_TEST(roots1 == roots2);

    _2 = _1;
    _2.SetView(Valuable::View::SupersetOfRoots);
    _2.optimize();
    _2.solve(a);

    _2 = _1;
    _2.SetView(Valuable::View::Solving);
    _2.optimize();
    roots2 = _2.solve(a);
    BOOST_TEST(roots1 == roots2);
}

BOOST_AUTO_TEST_CASE(SumCopy_test) {
    Sum sum;
    Valuable::OptimizeOff off; // ensure sum type not polimorphed into Integer
    sum += 1; // Whence optimizations on, Sum just became Integer (polymorphic typisation in action)
    sum += 2;
    sum += 3;

    Valuable s1 = sum; // Wraps morphed objects
    BOOST_TEST(sum == s1);
    BOOST_TEST(s1 == sum);

    auto s2 = sum; // If optimizations on, Sum::operator= called passing Integer& instead of Sum&
    BOOST_TEST(sum == s2);
    BOOST_TEST(s2 == sum);

    // sum.Reset(); // otherwise ~Sum would called on Integer object for optimizations on
}

BOOST_AUTO_TEST_CASE(Simple_SumOrderComparator_test)
{
    SumOrderComparator cmp;
    DECL_VA(v1);
    auto simple1 = v1 + 2;
    auto simple2 = v1 + 3;
    bool cmpSimple12 = cmp(simple1, simple2);
    bool cmpSimple21 = cmp(simple2, simple1);
    BOOST_TEST(cmpSimple12 != cmpSimple21);
}

BOOST_AUTO_TEST_CASE(Exponentiations_SumOrderComparator_test)
{
    SumOrderComparator cmp;
    DECL_VA(v1);
    auto med1 = (v1 ^ 2) + 5;
    auto med2 = (v1 ^ 2) + 7;
    bool cmpMed12 = cmp(med1, med2);
    bool cmpMed21 = cmp(med2, med1);
    BOOST_TEST(cmpMed12 != cmpMed21);
}

BOOST_AUTO_TEST_CASE(Exponentiations_into_variable_SumOrderComparator_test) {
    SumOrderComparator cmp;
    DECL_VA(v1);
    auto _1 = "((1/4)*((-1)^v1) + (19/4))"_v;
    bool cmp12 = cmp(_1, v1);
    bool cmp21 = cmp(v1, _1);
    BOOST_TEST(cmp12 != cmp21);

    _1 = "((-1) ^ ((1 / 4) * ((-1) ^ v1) + (19 / 4)))"_v;
    auto _2 = -1^v1;
    cmp12 = cmp(_1, _2);
    cmp21 = cmp(_2, _1);
    BOOST_TEST(cmp12 != cmp21);

    _1 = "((-1) / 4) * ((-1) ^ ((1 / 4) * ((-1) ^ v1) + (19 / 4)))"_v;
    _2 = "((-1)/4)*((-1)^v1)"_v;
    cmp12 = cmp(_1, _2);
    cmp21 = cmp(_2, _1);
    BOOST_TEST(cmp12 != cmp21);
}

BOOST_AUTO_TEST_CASE(Fractions_SumOrderComparator_test)
{
    SumOrderComparator cmp;
    DECL_VA(v1);
    auto _1 = (v1 ^ (1_v / 2)) + ((v1 + 1) / 2);
    auto _2 = (v1 ^ (1_v / 2)) + ((v1 + 2) / 2);
    bool cmp12 = cmp(_1, _2);
    bool cmp21 = cmp(_2, _1);
    BOOST_TEST(cmp12 != cmp21);

    _1 = "((-1 * ((-1) ^ x) + 1) / (-4))"_v;
    _2 = "((-1 * ((-1) ^ ((1 / 2) * x + ((-1 * ((-1) ^ x) + 1) / (-4)))) + 1) / (-2))"_v;
    cmp12 = cmp(_1, _2);
    cmp21 = cmp(_2, _1);
    BOOST_TEST(cmp12 != cmp21);
}

BOOST_AUTO_TEST_CASE(Fractions_with_Exponentiation_SumOrderComparator_test) {
    SumOrderComparator cmp;
    Fraction _1 = {-std::sqrt(constants::two), 2};
    auto _2 = "(((1/4)*(1r2) + ((-1)/2)*(1r2) + (((1r2))/4) + (1r2))^((1/2)))"_v;
    auto cmp12 = cmp(_1, _2);
    auto cmp21 = cmp(_2, _1);
    BOOST_TEST(cmp12 != cmp21);
}

BOOST_AUTO_TEST_CASE(AddMultipleEqual_SumOrderComparator_test)
{
    // test Sum move assignment operator:
    Valuable::OptimizeOff off;
    auto sum = std::move(Sum{2, 4});

    // test adding equal by value values:
    sum.Add(1_v / 2);
    sum.Add(2_v / 4);

    auto _1 = sum.Optimized();
    auto _2 = 7_v;
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(SumOrderComparator_test) {
    SumOrderComparator cmp;
    {
        DECL_VA(c);
        auto _1 = (((c ^ 4) + -72 * (c ^ 2) + 240 * c + -144) ^ ((1 / 2)));
        auto _2 = -6 * (((c ^ 4) + -72 * (c ^ 2) + 240 * c + -144) ^ ((1 / 2))) * (c ^ (-1));
        BOOST_TEST(cmp(_1, _2) != cmp(_2, _1));
    }
    {
        Variable va;
        auto _1 = ((va ^ 4) - 72 * (va ^ 2) + 240 * va - 144) ^ (1_v / 2);
        auto _2 = -6_v * (((va ^ 4) - 72 * (va ^ 2) + 240 * va + -144) ^ ((1_v / 2))) * (va ^ (-1));
        BOOST_TEST(cmp(_1, _2) != cmp(_2, _1));
    }
    DECL_VA(v1);
    {
        Valuable::OptimizeOff off;
        auto _1 =
            ((-8 *
                  ((-1) ^ ((1 / 4) * v1 + ((8 * ((-1) ^ ((1 / 2) * v1 + 5 + ((-1 * ((-1) ^ (v1 + 10)) + 1) / (-4)))) +
                                            4 * ((-1) ^ (v1 + 10)) + 68) /
                                           32))) *
                  ((-1) ^ ((1 / 4) * v1 + ((8 * ((-1) ^ ((1 / 2) * v1 + ((-2 * ((-1) ^ (v1 + 11)) + -42) / (-8)))) +
                                            4 * ((-1) ^ (v1 + 11)) + 76) /
                                           32))) +
              8 * ((-1) ^ ((1 / 4) * v1 + ((8 * ((-1) ^ ((1 / 2) * v1 + 5 + ((-1 * ((-1) ^ (v1 + 10)) + 1) / (-4)))) +
                                            4 * ((-1) ^ (v1 + 10)) + 68) /
                                           32))) +
              8 * ((-1) ^ ((1 / 4) * v1 + ((8 * ((-1) ^ ((1 / 2) * v1 + ((-2 * ((-1) ^ (v1 + 11)) + -42) / (-8)))) +
                                            4 * ((-1) ^ (v1 + 11)) + 76) /
                                           32))) +
              -8) /
             (-8));
        auto _2 =
            ((-32 * ((-1) ^ (v1 + 11)) * ((-1) ^ (v1 + 10)) +
              -64 * ((-1) ^ ((1 / 2) * v1 + 5 + ((-1 * ((-1) ^ (v1 + 10)) + 1) / (-4)))) *
                  ((-1) ^ ((1 / 2) * v1 + ((-2 * ((-1) ^ (v1 + 11)) + -42) / (-8)))) +
              -256 *
                  ((-1) ^ ((1 / 8) * v1 +
                           ((-8 * ((-1) ^ (v1 + 11)) +
                             -16 * ((-1) ^ ((1 / 2) * v1 + ((-2 * ((-1) ^ (v1 + 11)) + -42) / (-8)))) +
                             -32 * ((-1) ^ ((1 / 4) * v1 +
                                            ((8 * ((-1) ^ ((1 / 2) * v1 + ((-2 * ((-1) ^ (v1 + 11)) + -42) / (-8)))) +
                                              4 * ((-1) ^ (v1 + 11)) + 76) /
                                             32))) +
                             -120) /
                            (-128)))) *
                  ((-1) ^ ((1 / 8) * v1 +
                           ((-8 * ((-1) ^ (v1 + 10)) +
                             -16 * ((-1) ^ ((1 / 2) * v1 + 5 + ((-1 * ((-1) ^ (v1 + 10)) + 1) / (-4)))) +
                             -32 * ((-1) ^ ((1 / 4) * v1 +
                                            ((8 * ((-1) ^ ((1 / 2) * v1 + 5 + ((-1 * ((-1) ^ (v1 + 10)) + 1) / (-4)))) +
                                              4 * ((-1) ^ (v1 + 10)) + 68) /
                                             32))) +
                             -104) /
                            (-128)))) +
              256 * ((-1) ^ ((1 / 8) * v1 +
                             ((-8 * ((-1) ^ (v1 + 11)) +
                               -16 * ((-1) ^ ((1 / 2) * v1 + ((-2 * ((-1) ^ (v1 + 11)) + -42) / (-8)))) +
                               -32 * ((-1) ^ ((1 / 4) * v1 +
                                              ((8 * ((-1) ^ ((1 / 2) * v1 + ((-2 * ((-1) ^ (v1 + 11)) + -42) / (-8)))) +
                                                4 * ((-1) ^ (v1 + 11)) + 76) /
                                               32))) +
                               -120) /
                              (-128)))) +
              256 * ((-1) ^ ((1 / 8) * v1 +
                             ((-8 * ((-1) ^ (v1 + 10)) +
                               -16 * ((-1) ^ ((1 / 2) * v1 + 5 + ((-1 * ((-1) ^ (v1 + 10)) + 1) / (-4)))) +
                               -32 * ((-1) ^ ((1 / 4) * v1 + ((8 * ((-1) ^ ((1 / 2) * v1 + 5 +
                                                                            ((-1 * ((-1) ^ (v1 + 10)) + 1) / (-4)))) +
                                                               4 * ((-1) ^ (v1 + 10)) + 68) /
                                                              32))) +
                               -104) /
                              (-128)))) +
              64 * ((-1) ^ ((1 / 2) * v1 + 5 + ((-1 * ((-1) ^ (v1 + 10)) + 1) / (-4)))) +
              64 * ((-1) ^ ((1 / 2) * v1 + ((-2 * ((-1) ^ (v1 + 11)) + -42) / (-8)))) + 32 * ((-1) ^ (v1 + 11)) +
              32 * ((-1) ^ (v1 + 10)) + -352) /
             (-128));
        BOOST_TEST(cmp(_1, _2) != cmp(_2, _1));
    }
    {
        Valuable item("-4*(v1^4)");
        Sum sum = {item,
                   Valuable("((v1^4) + -4*(v1^3) + 6*(v1^2) + -4*v1 + 1)*((-1)/2)*(v1^2)"),
                   Valuable("(v1^5)"),
                   Valuable("6*(v1^3)"),
                   Valuable("-4*(v1^2)"),
                   Valuable("v1")};

        auto end = std::end(sum);
        auto it = std::find(std::begin(sum), end, item);
        auto found = it != end;
        BOOST_TEST(found);
    }
}

BOOST_AUTO_TEST_CASE(Mixed_SumOrderComparator_test)
{
    SumOrderComparator cmp;
    Valuable::OptimizeOff off;
    auto _1 = "((-8*((-1)^((1/4)*v1 + ((8*((-1)^((1/2)*v1 + 5 + ((-1*((-1)^(v1 + 10)) + 1)/(-4)))) + 4*((-1)^(v1 + 10)) + 68)/32)))*((-1)^((1/4)*v1 + ((8*((-1)^((1/2)*v1 + ((-2*((-1)^(v1 + 11)) + -42)/(-8)))) + 4*((-1)^(v1 + 11)) + 76)/32))) + 8*((-1)^((1/4)*v1 + ((8*((-1)^((1/2)*v1 + 5 + ((-1*((-1)^(v1 + 10)) + 1)/(-4)))) + 4*((-1)^(v1 + 10)) + 68)/32))) + 8*((-1)^((1/4)*v1 + ((8*((-1)^((1/2)*v1 + ((-2*((-1)^(v1 + 11)) + -42)/(-8)))) + 4*((-1)^(v1 + 11)) + 76)/32))) + -8)/(-8))"_v;
    auto _2 = "((-32*((-1)^(v1 + 11))*((-1)^(v1 + 10)) + -64*((-1)^((1/2)*v1 + 5 + ((-1*((-1)^(v1 + 10)) + 1)/(-4))))*((-1)^((1/2)*v1 + ((-2*((-1)^(v1 + 11)) + -42)/(-8)))) + -256*((-1)^((1/8)*v1 + ((-8*((-1)^(v1 + 11)) + -16*((-1)^((1/2)*v1 + ((-2*((-1)^(v1 + 11)) + -42)/(-8)))) + -32*((-1)^((1/4)*v1 + ((8*((-1)^((1/2)*v1 + ((-2*((-1)^(v1 + 11)) + -42)/(-8)))) + 4*((-1)^(v1 + 11)) + 76)/32))) + -120)/(-128))))*((-1)^((1/8)*v1 + ((-8*((-1)^(v1 + 10)) + -16*((-1)^((1/2)*v1 + 5 + ((-1*((-1)^(v1 + 10)) + 1)/(-4)))) + -32*((-1)^((1/4)*v1 + ((8*((-1)^((1/2)*v1 + 5 + ((-1*((-1)^(v1 + 10)) + 1)/(-4)))) + 4*((-1)^(v1 + 10)) + 68)/32))) + -104)/(-128)))) + 256*((-1)^((1/8)*v1 + ((-8*((-1)^(v1 + 11)) + -16*((-1)^((1/2)*v1 + ((-2*((-1)^(v1 + 11)) + -42)/(-8)))) + -32*((-1)^((1/4)*v1 + ((8*((-1)^((1/2)*v1 + ((-2*((-1)^(v1 + 11)) + -42)/(-8)))) + 4*((-1)^(v1 + 11)) + 76)/32))) + -120)/(-128)))) + 256*((-1)^((1/8)*v1 + ((-8*((-1)^(v1 + 10)) + -16*((-1)^((1/2)*v1 + 5 + ((-1*((-1)^(v1 + 10)) + 1)/(-4)))) + -32*((-1)^((1/4)*v1 + ((8*((-1)^((1/2)*v1 + 5 + ((-1*((-1)^(v1 + 10)) + 1)/(-4)))) + 4*((-1)^(v1 + 10)) + 68)/32))) + -104)/(-128)))) + 64*((-1)^((1/2)*v1 + 5 + ((-1*((-1)^(v1 + 10)) + 1)/(-4)))) + 64*((-1)^((1/2)*v1 + ((-2*((-1)^(v1 + 11)) + -42)/(-8)))) + 32*((-1)^(v1 + 11)) + 32*((-1)^(v1 + 10)) + -352)/(-128))"_v;
    BOOST_TEST(cmp(_1, _2) != cmp(_2, _1));

    _1 = "(8/32)*((-1)^(((-2)/(-8))*((-1)^(v1 + 11)) + ((-42)/(-8))))"_v;
    _2 = "(1/4)*((-1)^v1)"_v;
    BOOST_TEST(cmp(_1, _2) != cmp(_2, _1));
}

BOOST_AUTO_TEST_CASE(SumToProductOrderComparator_test)
{
    auto _1 = "(-256*(x^6) + 2048*(x^4))"_v;
    auto _2 = "16777216*(x^12)"_v;
    InequalOrderCheck(_1, _2);
}

BOOST_AUTO_TEST_CASE(SumFindMember_test) {
    auto _1 = "-4*(v1^4)"_v;
    auto _2 = "((v1^4) + -4*(v1^3) + 6*(v1^2) + -4*v1 + 1)*((-1)/2)*(v1^2)"_v;
    Sum sum = {_1, _2, "(v1^5)"_v, "6*(v1^3)"_v, "-4*(v1^2)"_v, "v1"_v};

    auto end = std::end(sum);
    auto it = std::find(std::begin(sum), end, _1);
    auto found = it != end;
    BOOST_TEST(found);

    it = std::find(std::begin(sum), end, _2);
    found = it != end;
    BOOST_TEST(!found); // _2 members distributed into sum members
}

BOOST_AUTO_TEST_CASE(Sum_coefficient_test)
{
    DECL_VA(x);
    
    // Test polynomial coefficient handling
    auto _1 = -1_v * x + x * x;
    auto _2 = (x ^ 2) - x;
    BOOST_TEST(_1 == _2);  // Equivalent polynomial forms
    
    // Test mixed coefficient types in polynomials
    auto _3 = Fraction(1, 2) * x + x * x;
    auto _4 = (x ^ 2) + Fraction(1, 2) * x;
    BOOST_TEST(_3 == _4);  // Order independence in polynomials
    
    // Test coefficient normalization in polynomials
    auto _5 = (-1_v * x + x * x) * (-1_v);
    auto _6 = x - (x ^ 2);
    BOOST_TEST(_5 == _6);  // Sign distribution
}

BOOST_AUTO_TEST_CASE(Sum_tests) {
    DECL_VARS(v1, v2, v3);

    auto _1 = (v1 + -1) * v1;
    auto _2 = (v1 ^ 2) + -1 * v1;
    BOOST_TEST(_1 == _2);

    _1 = Sum{1, -1};
    BOOST_TEST(_1.IsZero());

    auto val1 = -2_v / 3 * v1;
    val1 += v1 * v1;
    val1 += 1_v / 9;
    auto val2 = 1_v / 9 + v1 * v1 + -2_v / 3 * v1;
    std::cout << val1 << " == " << val2 << std::endl;
    BOOST_TEST((-2_v / 3 * v1 + v1 * v1 + 1_v / 9) == (1_v / 9 + v1 * v1 + -2_v / 3 * v1));
    BOOST_TEST(1_v / 9 - (2_v / 3) * v3 + v3 * v3 == 1_v / 9 - (2_v / 3) * v3 + v3 * v3);

    auto t = v3 - 1;
    BOOST_TEST(((v3 - 1) ^ 2) == t ^ 2);
    auto sq = t * t;
    sq.optimize();
    BOOST_TEST(sq == t ^ 2);
    BOOST_TEST(sq == (v3 ^ 2) - v3 * 2 + 1);
    _1 = sq / t;
    BOOST_TEST(_1 == t);

    Valuable f = 1_v / 2, f1 = 5_v / 6, a = 1_v + 2 + f + 3, b = 6_v + f1;

    std::cout << a << std::endl;
    a -= b;
    std::cout << a << std::endl;

    auto s = a;
    s += v3;
    std::cout << s;
    BOOST_TEST(s == a + v3);
    std::cout << s << std::endl;
    ohashes(s);
    ohashes(-1_v / 3 + v1);
    std::cout << (-2_v / 3 * v1).Hash() << std::endl;
    s = s * s;
    std::cout << s << std::endl;
    ohashes(s);
    s.optimize();
    ohashes(s);
    t = 1_v / 9 - (2_v / 3) * v3 + v3 * v3;
    ohashes(t);
    BOOST_TEST(s == 1_v / 9 - (2_v / 3) * v3 + v3 * v3);

    BOOST_TEST((v1 * v2 + v1 * v1 * v2 + 1 / 3 * 125 * v1 + 1 / 3 * 125 * v1 * v1) ==
               (v1 * v2 + 1 / 3 * 125 * v1 + 1 / 3 * 125 * v1 * v1 + v1 * v1 * v2));
    //    BOOST_TEST((((65851823091255177969664_v*v11*v11*v11*v11*v11 +
    //    -4433312658227724288*v11*v11*v11*v11*v11*v11))/(v11*v11*v11*v11*v11*v11*v11*v11*v11*v11*v11) +
    //    v9*v9*-23702740992/(v11*v11*v11*v11*v11) + v10*v10*-23702740992/(v11*v11*v11*v11*v11)) ==
    //    (((65851823091255177969664*v11*v11*v11*v11*v11 +
    //    -4433312658227724288*v11*v11*v11*v11*v11*v11))/(v11*v11*v11*v11*v11*v11*v11*v11*v11*v11*v11) +
    //    v9*v9*-23702740992/(v11*v11*v11*v11*v11) + v10*v10*-23702740992/(v11*v11*v11*v11*v11)))
    s = (v3 + 1) + (v1 + v2);
    s.optimize();
    BOOST_TEST(s.IsSum());
    auto& sc = s.as<Sum>();
    BOOST_TEST(sc.size() == 4);
    BOOST_TEST((((8_v + (((8 * 1 + 8 * 1 + -8 + (1_v ^ 2) * -4 + (1_v ^ 2) * -4)) ^ ((1_v / 2))))) / 2) == 4);

    DECL_VA(x);
    DECL_VA(y);
    DECL_VA(z);
    _1 = (x + y) * (x - y);
    _2 = (x ^ 2) - (y ^ 2);
    BOOST_TEST(_1 == _2);
    sq = (x ^ 4) + (z ^ 4);
    BOOST_TEST(sq.IsSum());
    auto sum = &sq.as<Sum>();
    if (sum) {
        BOOST_TEST(sum->size() == 2);
    }
    t = -2 * y - 2 * x;
    BOOST_TEST(t.IsSum());
    sum = &t.as<Sum>();
    if (sum) {
        BOOST_TEST(sum->size() == 2);
    }
    sq =
        "(x ^ 4) + (z ^ 4) + (y ^ 4) - 4 * x * (z ^ 2) - 4 * (x ^ 2) * y - 4 * x * (y ^ 2) - 4 * y * (z ^ 2) -16 * (x ^ 2) * z - 16 * (y ^ 2) * z - 72 * y - 72 * x + 40 * (x ^ 2) + 40 * (y ^ 2) + 8 * x * y + 32 * x * z +         32 * y * z - 4 * (y ^ 3) - 4 * (x ^ 3) + 2 * (x ^ 2) * (y ^ 2) + 2 * (y ^ 2) * (z ^ 2) +         2 * (x ^ 2) * (z ^ 2) - 16 * (z ^ 3) + 100 * (z ^ 2) - 288 * z + 324 "_v;
    _1 = sq;
    sq = Valuable(
        "(x ^ 4) + (z ^ 4) + (y ^ 4) - 4 * x * (z ^ 2) - 4 * (x ^ 2) * y - 4 * x * (y ^ 2) - 4 * y * (z ^ 2) -16 * (x ^ 2) * z - 16 * (y ^ 2) * z - 72 * y - 72 * x + 40 * (x ^ 2) + 40 * (y ^ 2) + 8 * x * y + 32 * x * z +         32 * y * z - 4 * (y ^ 3) - 4 * (x ^ 3) + 2 * (x ^ 2) * (y ^ 2) + 2 * (y ^ 2) * (z ^ 2) +         2 * (x ^ 2) * (z ^ 2) - 16 * (z ^ 3) + 100 * (z ^ 2) - 288 * z + 324 "sv,
        {{"x", x}, {"y", y}, {"z", z}}, true);
    _2 = sq;
    auto ok = _1 == _2;
    BOOST_TEST(ok);
    BOOST_TEST(sq.IsSum());
    sum = &sq.as<Sum>();
    if (sum->size() != 25) {
        std::cout << sum->str() << std::endl;
        BOOST_TEST(sum->size() == 25);
    }

    auto sqc = sq;
    sqc.optimize();
    BOOST_TEST(sqc == sq);

    auto _ = (z ^ 2) - 2 * y + (y ^ 2) - 2 * x + (x ^ 2) - 8 * z + 18;
    BOOST_TEST(_.Sq() == sq);

    BOOST_TEST(_.IsSum());
    sum = &_.as<Sum>();
    BOOST_TEST(sum->size() == 7);
    t = _;
    t.Eval(x, 1);
    t.Eval(y, 1);
    t.Eval(z, 4);
    t.optimize();
    BOOST_TEST(t == 0);
}

BOOST_AUTO_TEST_CASE(Sum_optimization_tests) {
    DECL_VARS(v1, v2, v3);
    auto value = (((v3 + -255) ^ 2) + ((v2 + 0) ^ 2) + ((v1 + -1) ^ 2)) *
                 (((v3 + -255) ^ 2) + ((v2 + -1) ^ 2) + ((v1 + 0) ^ 2)) *
                 (((v3 - 255) ^ 2) + ((v2 + -1) ^ 2) + ((v1 + -1) ^ 2));
    value.optimize();
}

BOOST_AUTO_TEST_CASE(Sum_advanced_tests
    , *disabled()
) {
    DECL_VARS(v1, v2, v3, x ,y ,z);

	auto _1 = -1 * v1 + 100;
    auto _2 = -1 * v2 / _1 + v3 / v1;
	_2 *= _1; // FIXME: bug
    auto _ = -1 * v2 + v3 * _1 / v1;
    BOOST_TEST(_2 == _);
    if (_2 != _) {
        _2.optimize();
        BOOST_TEST(_2 == _);
    }
	_1 = -1 * v1 + 100;
    _2 = -1 * v2 / _1 + v3 / v1;
	_1 *= _2;
    BOOST_TEST(_1 == _);
    auto sqc = Valuable(
        "(x^4)+(z^4)+(y^4)-4*x*(z^2)-4*(x^2)*y-4*x*(y^2)-4*y*(z^2)-16*(x^2)*z-16*(y^2)*z-72*y-72*x+40*(x^2)+40*(y^2)+8*x*y+32*x*z+32*y*z-4*(y^3)-4*(x^3)+2*(x^2)*(y^2)+2*(y^2)*(z^2)+2*(x^2)*(z^2)-16*(z^3)+100*(z^2)-288*z+324"sv,
        {{"x", x}, {"y", y}, {"z", z}}, true);
    auto t = sqc;
    t.Eval(x,1); t.Eval(y,1); t.Eval(z, 4);
    t.optimize();
    BOOST_TEST(t==0);
    
    DECL_VA(v4);
    DECL_VA(v5);
    DECL_VA(v6);
    // shuffled compare test
    BOOST_TEST((v5^2)*-4*v4 == (v5^2)*-4*v4);
    BOOST_TEST(v4+v5 == v5+v4);
    BOOST_TEST(v4*v6+v5*v6 == v5*v6+v4*v6);
    
    auto t1 = (v4^2)*v5*-4 + (v5^2)*-4*v4;
    auto t2 = (v5^2)*-4*v4 + (v4^2)*v5*-4;
    auto& s1 = t1.as<Sum>();
    auto& s2 = t2.as<Sum>();
    BOOST_TEST(s1.size()==s2.size());
    BOOST_TEST(s1.size()==2);
//    BOOST_TEST(*s1->begin()==*s2->GetConstCont().rbegin());
//    BOOST_TEST(*s2->begin()==*s1->GetConstCont().rbegin());
    BOOST_TEST(t1 == t2);
    
    t1 = Valuable("((v4^4) + (v6^4) + (v5^4) + (v6^2)*-4*v4 + (v4^2)*v5*-4 + (v5^2)*-4*v4 + (v6^2)*v5*-4 + (v4^2)*-16*v6 + (v5^2)*-16*v6 + -72*v5 + -72*v4 + -288*v6 + 324 + (v6^2)*100 + (v4^2)*40 + (v5^2)*40 + 8*v5*v4 + (v6^3)*-16 + 32*v6*v4 + 32*v5*v6 + (v5^3)*-4 + (v4^3)*-4 + (v6^2)*(v5^2)*2 + (v5^2)*(v4^2)*2 + (v6^2)*(v4^2)*2)", v4.getVaHost(), true);
    t2 = Valuable("((v4^4) + (v6^4) + (v5^4) + (v6^2)*-4*v4 + (v5^2)*-4*v4 + (v4^2)*v5*-4 + (v6^2)*v5*-4 + (v4^2)*-16*v6 + (v5^2)*-16*v6 + -72*v5 + -72*v4 + -288*v6 + 324 + (v6^2)*100 + (v4^2)*40 + (v5^2)*40 + 8*v5*v4 + (v6^3)*-16 + 32*v6*v4 + 32*v5*v6 + (v5^3)*-4 + (v4^3)*-4 + (v6^2)*(v5^2)*2 + (v5^2)*(v4^2)*2 + (v6^2)*(v4^2)*2)", v4.getVaHost(), true);
    BOOST_TEST(t1 == t2);
    t = _;
    t *= t;
    BOOST_TEST(t==sqc);
    t.Eval(x,1); t.Eval(y,1); t.Eval(z, 4);
    t.optimize();
    BOOST_TEST(t==0);

    _1 = v3 + (2040_v/v1);
    _2 = (-1_v/v1)*v2;
    _ = _1 / _2;
    {
    auto ss = _.Solutions(v3);
    BOOST_TEST(ss.size() == 1);
    BOOST_TEST(!ss.begin()->HasVa(v3));
    auto _ = *ss.begin();
    BOOST_TEST(_ == -2040_v/v1);
    }
    {
    auto ss = _(v3);
    BOOST_TEST(!ss.HasVa(v3));
    BOOST_TEST(ss == -2040_v/v1);
    }
    
    _1 = x*-1+y*-2;
    _1 /= z;
    BOOST_TEST(_1.HasVa(z));
    _2 = x*-1/z+y*-2/z;
    BOOST_TEST(_1==_2);
    
    DECL_VARS(v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17);
    
    _1 = -1_v*v17*v12 + -1_v*v16*v13 + 174;
    _2 = -1_v*v17*v12 + -1_v*v13*v16 + 174;
    BOOST_TEST(_1 == _2);

    _1 = -1_v*(v11^2)*v16 + -1_v*v17*v10*v11 + -1_v*v15*v12*v11 + -1_v*v14*v13*v11 + 2723_v*v11;
    _2 = -1_v*(v11^2)*v16 + -1_v*v13*v11*v14 + -1_v*v17*v10*v11 + -1_v*v15*v12*v11 + 2723_v*v11;
    BOOST_TEST(_1 == _2);

    _ = (-1_v*(v6^-1_v)*(v8^2_v)*v13 + -1_v*(v6^-1_v)*v14*v7*v8 + -1_v*(v6^-1)*v12*v9*v8 + v10*v13 + v9*v14 + -1_v*(v6^-1)*v11*v10*v8 + 2723_v*(v6^-1)*v8 + -174_v);
    // -1_v*(v6^-1_v)*(v8^2_v)*v13 + -1_v*(v6^-1_v)*v14*v7*v8 + -1_v*(v6^-1)*v12*v9*v8 -1_v*(v6^-1)*v11*v10*v8 + 2723_v*(v6^-1)*v8
    //              == -v10*v13 - v9*v14 + 174_v
    
    // -1_v*(v8^2_v)*v13 + -1_v*v14*v7*v8 + -1_v*v12*v9*v8 -1_v*v11*v10*v8 + 2723_v*v8 == v6(-v10*v13 - v9*v14 + 174_v)
    // v6 == (-1_v*(v8^2_v)*v13 + -1_v*v14*v7*v8 + -1_v*v12*v9*v8 -1_v*v11*v10*v8 + 2723_v*v8) / (-v10*v13 - v9*v14 + 174_v)
    _1 = _(v6);
    _2 = Fraction((v8^2_v)*v13 + v14*v7*v8 + v12*v9*v8 + v11*v10*v8 - 2723_v*v8,
                  v10*v13 + v9*v14 - 174_v);
    BOOST_TEST(_1 == _2);
    
    _ = ((-1_v/2)*(v14^-1)*v13*v9 + (-1_v/2)*(v14^-1)*v12*v10 + (-1_v/2)*(((4*v14*v13*v10*v7 + 4*(v14^2)*v9*v7 + 2*v13*v12*v10*v9 + (v9^2)*(v13^2) + (v10^2)*(v12^2) + 1732*v13*v9 + 1732*v12*v10 + -696*v14*v7 + 749956))^((1_v/2)))*(v14^-1));
    _ += -433*(v14^-1);
    
    _1 = (x^2)*y + (y^2)*x + x;
    _2 = (y^2)*x + (x^2)*y + x;
    BOOST_TEST(_1 == _2);

    _1 = (x^3)+(x^4)*y+(x^3)*(y^2)+y*x+(x^2)*(y^2)+x*(y^3);
    _2 = (x^2) + y;
    _ = x+(x^2)*y+(y^2)*x;
    auto s = _1 / _2;
    BOOST_TEST(s == _);
    s *= _2;
    BOOST_TEST(s == _1);
    
    _1 = (x^6)+(x^4)*6+(x^3)+(x^2)*9+x*3-5;
    _ = _1;
    _.Eval(x,y);
    _1 -= _;
    _2 = x-y;
    s = _1 / _2;
    _ = (y^5)+(x^5)+(y^4)*x+(x^4)*y+(y^3)*(x^2)+(x^3)*(y^2)+6*(y^3)+6*(x^3)+6*(y^2)*x+6*(x^2)*y+(y^2)+(x^2)+y*x+9*y+9*x+3;
    BOOST_TEST(s == _);
    
    _ = ((((v3 + -255))^2) + (((v2 + 0))^2) + (((v1 + -1))^2))*((((v3 + -255))^2) + (((v2 + -1))^2) + (((v1 + 0))^2))*((((v3 + -255))^2) + (((v2 + -1))^2) + (((v1 + -1))^2));
    _.optimize();
    
    _1 = (-510*v3 + (v3^2) + (v2^2) + (v1^2) + 65025);
    _2 = ((v3^2) + (v2^2) + (v1^2) + -510*v3 + 65025);
    BOOST_TEST(_1 == _2);

    _1 = 5120_v*(v5 ^ 7);
    _2 = 2 * v5 + v7 + 3;
    _ = Sum{ _1 } / _2;
    BOOST_TEST(_ == _1 / _2);
    
    _ = (x-1)/(x-3);
    BOOST_TEST(_ == 1+2/(x-3));
    
    _ = (4*(x^3) + -8*(x^2) + -27*x + 45) / (x-3);
    BOOST_TEST(_ == (4*(x^2)+4*x-15));
    
    _ = (4*(x^3) + -8*(x^2) + -27*x + 45);
    _.SetView(Valuable::View::Solving);
    _ /= x-3;
    BOOST_TEST(_ == (4*(x^2)+4*x-15));
    
    auto v = "v"_va;
    _ = (4_v*(v^2) + 2048)^(1_v/2);
    _.SetView(Valuable::View::Solving);
    _.optimize();
    t=4_v;
    auto c=2048_v;
    auto a = "a"_va;
    
    
    
    _ = (32_v*((4_v*(v^2) + 2048)^((1_v/2)))*((4_v*(v^2) + 2044)^((1_v/2)))*((4_v*(v^2) + 1948)^((1_v/2))) + -15604_v*((4_v*(v^2) + 2032)^((1_v/2)))*((4_v*(v^2) + 892)^((1_v/2))));
    auto sum = &_.as<Sum>();
    BOOST_TEST(*sum == _);
    // TODO:    _ = _(v);
    
}

class Pub : public Valuable
{
    using base = Valuable;
public:
    auto e() { return getInst(); }
    void so(bool o) { optimized = o; }
    using base::get;
};

BOOST_AUTO_TEST_CASE(Become_tests)
{
    Variable v;
    Valuable f = 1_v/2;
    auto s = f+v;
    auto pub = reinterpret_cast<Pub*>(&s);
    BOOST_TEST(pub->e());
    auto isValuableType = typeid(s)==typeid(Valuable);
    BOOST_TEST(isValuableType);
    auto& pubValue = pub->get();
    auto isInnerSum = typeid(pubValue) == typeid(Sum);
    BOOST_TEST(isInnerSum);
    isInnerSum = s.Type()==typeid(Sum);
    BOOST_TEST(isInnerSum);
}

BOOST_AUTO_TEST_CASE(Solution_tests)
{
    auto _ = (-1*(4_v^((1_v/2))) + (21_v/8)) ^ 2;
    DECL_VA(v);
    auto sol = ((v^2)-4_v).Solutions(v);
    using sol_t = decltype(sol);
    BOOST_TEST(sol == sol_t({2,-2}));
 
    auto _1 = v.Equals(1.5);
    auto _2 = v.Equals(-2.5);
    auto _3 = v.Equals(3);
    auto equation = _1.LogicOr(_2).LogicOr(_3);
    sol = equation.Solutions();
    BOOST_TEST(sol == sol_t({1.5,-2.5,3}));
    
    _1 = 1.5;
    _2 = -2.5;
    _3 = -17_v/8;
    equation = v.Equals(_1)
                .LogicOr(v.Equals(_2))
                .LogicOr(v.Equals(_3));
    sol = equation.Solutions();
    BOOST_TEST(sol == sol_t({_1,_2,_3}));
    //auto x = equation(v);
    //reinterpret_cast<Pub*>(&reinterpret_cast<Pub*>(&x)->get())->so({});
    //x.optimize(); //TODO : complete optimization for uncomment
//    BOOST_TEST(s == x); // TODO: uncomment when complete optimization of x
    
    equation = (v-1)*(v-2)*(v-3);
    sol = equation.Solutions();
    BOOST_TEST(sol == sol_t({1,2,3}));

    equation *= v+4;
    sol = equation.Solutions();
    BOOST_TEST(sol == sol_t({1,2,3,-4}));    
}

BOOST_AUTO_TEST_CASE(Solve_bsquare_test_no_hang
                     ,*timeout(2)
                     )
{
    DECL_VA(x);
    auto _ = (x^4) + 868*(x^2) + 188356;
    auto solutions = _.Solutions();
    for (auto& s : solutions) {
        auto is_a_root = _.Test(x, s);
        BOOST_TEST(is_a_root);
    }
}

BOOST_AUTO_TEST_CASE(Radical_Solution_tests
                     , *disabled() // TODO: implement proper radicals support
                     )
{
    DECL_VA(v);
	auto equation = ((v ^ 4) + (13_v / 8) * (v ^ 3) + ((-101_v) / 16) * (v ^ 2) + ((-735_v) / 128) * v + (765_v / 64));
    auto sol = equation.Solutions();
    BOOST_TEST(sol.size() == 4);
	// TODO: radical solutions test values
}

BOOST_AUTO_TEST_CASE(Containers_test)
{
    Variable va;
    SumOrderComparator cmp;
    auto _1 = ((va ^ 4) - 72 * (va ^ 2) + 240 * va - 144) ^ (1_v / 2);
    auto _2 = -6_v * (((va ^ 4) - 72 * (va ^ 2) + 240 * va + -144) ^ ((1_v / 2))) * (va ^ (-1));
    BOOST_TEST(cmp(_1, _2) != cmp(_2, _1));

    std::vector<Valuable> vec{0_v, va, va+1_v};
    std::set<Valuable, SumOrderComparator> s;
    std::unordered_set<Valuable> us;
    for(auto& a : vec)
    {
        s.insert(a);
        us.insert(a);
    }
    BOOST_TEST(vec.size()==3);
    BOOST_TEST(s.size()==3);
    BOOST_TEST(us.size()==3);

    DECL_VA(b);
    DECL_VA(c);
    s.clear();

    s.insert(b ^ 2);
    auto it = s.find(c ^ 2);
}

BOOST_AUTO_TEST_CASE(PolynomialDivHang_test_no_hang
                     ,*timeout(2)
                     )
{
    Variable va;
    
    // good
    auto _1 = (va^2)-va*2+1;
    auto _2 = va-1;
    BOOST_TEST(_1/_2 == _2);
    
    // hang -> should fall back to fraction
    _2 = (va^2)-1;
    auto _ = _1/_2; // hang
//    BOOST_TEST(_.IsFraction());
    BOOST_TEST(_1/_2 == (((va^2)-va*2+1)/((va^2)-1)));

	auto p = (va + 5) / (va + 1);
	p.Eval(va, 2);
	p.optimize();
	BOOST_TEST(7_v / 3 == p);
}

BOOST_AUTO_TEST_CASE(Modulo_test)
{
    Variable va;
    auto _ = (va + 5) / (va + 1);
    auto p = (va + 5) % (va + 1);
    BOOST_TEST(p == (4_v % (va + 1)));
    p.Eval(va, 2);
    p.optimize();
    BOOST_TEST(7_v % 3 == p);
}

BOOST_AUTO_TEST_CASE(_1_2_3_) {
    DECL_VA(x);
    auto expr = x.Equals(1) || x.Equals(2) || x.Equals(3);
    std::cout
        << "expr = x.Equals(1) || x.Equals(2) || x.Equals(3)" << std::endl
        << expr << std::endl
        << "Solutions:" << std::endl;
    auto solutions = expr.Solutions();
    decltype(solutions) solutionsEtalon = { 1, 2, 3 };
    for (auto&& s : solutions) {
        std::cout << '\t' << s << std::endl;
    }
    BOOST_TEST(solutions == solutionsEtalon);
    std::cout << std::endl;
    // TODO : // FIXME: fix this
    auto solution = expr(x);
    std::cout << "Multival for 1, 2, 3 is " << solution << std::endl;
    std::cout << "Distinct of multival:" << std::endl;
    solutions = solution.Distinct();
    for (auto&& s : solutions) {
        std::cout << '\t' << s << std::endl;
    }
    BOOST_TEST(solutions == solutionsEtalon);
    std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(VirtualIterationTest
    , *disabled()
) {
    DECL_VA(n);
    DECL_VA(i);

//    auto sum = (i.MustBeInt() && i.Less(1000) && constants::zero.Less(i)).SumOfRoots();
    auto e = i ^ n;
    for (auto i = 1000; i-->0 ;)
    {
        
    }
    
    // Define a Valuable object representing the integrand t^(z-1)e^(-t)
    DECL_VA(t);
    DECL_VA(z);
    Valuable integrand = t ^ (z - 1) * constants::e ^ -t;

    // Evaluate the indefinite integral at the limits 0 and +infinity
    Valuable definiteIntegral = integrand.Integral(t, constants::zero, constants::infinity);

    // Output the result
    std::cout << "The definite integral of t^(z-1)e^(-t) from 0 to +infinity is: " << definiteIntegral << std::endl;
    auto forSix = definiteIntegral.Eval({{z, 6_v}});
    std::cout << 6 << " : " << forSix << std::endl;
}

BOOST_AUTO_TEST_CASE(GCDTest) {
    DECL_VARS(x, a, b, c, d);
    auto _1 = -4 * (a ^ 2) + -4 * (b ^ 2) + -4 * (c ^ 2) + -8 * b * c + 280 * b + 280 * c + 40 * a + -5000;
    auto varless1 = _1.varless();
    BOOST_TEST(varless1 == 4_v);
    auto _2 = -4 * (a ^ 2) + -4 * (d ^ 2) + 280 * d + 40 * a + -5000;
    auto varless2 = _2.varless();
    BOOST_TEST(varless2 == 4_v);
    Valuable gcd = 4;
    auto _ = _1.GCD(_2);
    BOOST_TEST(_ == gcd);
    _ = _2.GCD(_1);
    BOOST_TEST(_ == gcd);

    _1 = (x ^ 2) - 4;
    _2 = (x ^ 2) + x - 6;
    gcd = x - 2;
    // FIXME : _ = _1.GCD(_2);
    //BOOST_TEST(_ == gcd);
}
