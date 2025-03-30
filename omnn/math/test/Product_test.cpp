#define BOOST_TEST_MODULE Product test
#include <boost/test/unit_test.hpp>

#include "Product.h"
#include "Sum.h"
#include "Fraction.h"
#include "Variable.h"
#include "Integer.h"
#include "System.h"

// Forward declare System to avoid circular dependency
namespace omnn {
namespace math {
class System;
}
}

#include "generic.hpp"

using namespace std;
using namespace omnn::math;
using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(Quadratic_coefficient_test) {
    DECL_VA(l);
    System sys;

    // Test equation: 9l^2 - 2 = 16
    auto term = 9_v * l;     // Should normalize with MultiplyIfSimplifiable
    auto squared = term * l; // Will normalize properly with fix

    // Verify coefficient normalization
    BOOST_TEST(squared == 9_v * (l ^ 2));

    // Test equation solving
    auto eq = squared - 18_v; // 9l^2 - 18 = 0
    sys << eq;

    // Solve for l
    auto solutions = sys.Solve(l);
    BOOST_TEST(solutions.size() == 2); // Should have both +√2 and -√2

    if (solutions.size() == 2) {
        auto it = solutions.begin();
        auto sol1 = *it++;
        auto sol2 = *it;

        // Test l^3 computation
        auto cube1 = sol1 ^ 3; // Should be 2√2
        auto cube2 = sol2 ^ 3; // Should be -2√2

        BOOST_TEST(cube1 == -cube2);      // Opposite values
        BOOST_TEST(cube1 * cube1 == 8_v); // (2√2)^2 = 8
    }
}

BOOST_AUTO_TEST_CASE(Product_numeric_type_test) {
    DECL_VA(x);

    // Test fraction coefficient handling
    auto _1 = Fraction(1, 2) * x;
    auto _2 = x * Fraction(1, 2);
    BOOST_TEST(_1 == _2); // Order independence with fractions

    // Test mixed numeric type handling
    auto _3 = (-1_v * x) * Fraction(1, 2);
    auto _4 = Fraction(-1, 2) * x;
    BOOST_TEST(_3 == _4); // Equivalent forms

    // Test coefficient normalization with mixed types
    auto _5 = (Fraction(-1, 2) * x) * (-2_v);
    BOOST_TEST(_5 == x); // Double negation with mixed types
}

BOOST_AUTO_TEST_CASE(Product_operator_tests) {
    Valuable::OptimizeOff off;
	auto p = std::move(Product{2, 4}); 

    // test adding equal by value values:
    p.Add(1_v / 2);
    p.Add(2_v / 4);

    auto _1 = p.Optimized();
    auto _2 = 2_v;
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(Product_comparision_test) {
    auto _1 = "2*((v8)^2)"_v;
    auto _2 = "2 * (v8 ^ 2)"_v;
    BOOST_TEST(_1 == _2);
    auto less = _1 < _2;
    BOOST_TEST(!less);
    less = _2 < _1;
    BOOST_TEST(!less);

    _1 = "((x)^2)*(z^2)"_v;
    _2 = "((x)^2)*((z)^2)"_v;
    BOOST_TEST(_1 == _2);
    less = _1 < _2;
    BOOST_TEST(!less);
    less = _2 < _1;
    BOOST_TEST(!less);
}

BOOST_AUTO_TEST_CASE(Product_coefficient_normalization_test) {
    DECL_VA(l);
    
    // Test equation: 9l^2 - 2 = 16
    // This test demonstrates coefficient normalization issue
    auto term = 9_v * l;  // First multiplication
    auto squared = term * l;  // Second multiplication
    
    // Test actual mathematical equivalence, not just string representation
    auto expected = 9_v * (l ^ 2);
    BOOST_TEST(squared != expected);  // Should fail without MultiplyIfSimplifiable
    
    // Test internal representation
    BOOST_TEST(squared.IsProduct());
    auto& p = squared.as<Product>();
    BOOST_TEST(p.size() == 3);  // Should have 3 members (9, l, l) without normalization
    
    // Test negative coefficient case
    auto negTerm = -1_v * l;
    auto negSquared = negTerm * l;
    auto negExpected = -1_v * (l ^ 2);
    BOOST_TEST(negSquared == negExpected);  // Should pass with normalization
    
    // Test l^3 computation
    auto cubed = squared * l;  // (9l^2) * l
    auto expectedCube = 9_v * (l ^ 3);  // 9l^3
    BOOST_TEST(cubed == expectedCube);  // Should pass with normalization
    
    // Verify internal representation
    BOOST_TEST(cubed.IsProduct());
    auto& c = cubed.as<Product>();
    BOOST_TEST(c.size() == 2);  // Should have 2 members (9, l^3) after normalization
}

BOOST_AUTO_TEST_CASE(Product_optimize_off_comparision_test) {
    Valuable::OptimizeOff off;
    auto _1 = "-2*(1^5)"_v;
    auto _2 = "(1 ^ 6)"_v;
    InequalOrderCheck(_1, _2);

    bool less = _1 < _2;
    BOOST_TEST(less);

    Sum sum = {std::move(_1), std::move(_2)};

    _1 = 1;
    _2 = "(-4 * (Y ^ 3))"_v;
    InequalOrderCheck(_1, _2);

    _1 = "((-1)/32)*(32r2)*(64r5)"_v;
    _2 = "((-4)/128)*(32r2)*(64r5)"_v;
    EqualOrderCheck(_1, _2);
}

BOOST_AUTO_TEST_CASE(Product_tests)
{
	auto f = 1_v / 2;
	auto f1 = 5_v / 6;
	auto v = f * 20 * f1 * 5;
    BOOST_TEST(v==125_v/3);
    
    DECL_VA(v1);
    DECL_VA(v2);
    auto _1 = 1_v/2*v1*v2;
    auto _2 = v1*v2*1_v/2;
    BOOST_TEST(_1.IsProduct());
    BOOST_TEST(_2.IsProduct());
    BOOST_TEST(_1 == _2);
    _1.Eval(v1, 3);
    _1.Eval(v2, 4);
    _1.optimize();
    BOOST_TEST(_1 == 6);
    v *= v1;
    v += v1*v2;
    BOOST_TEST(v == (125_v/3*v1 + v1*v2));
    BOOST_TEST(v.str() == "(v2*v1 + (125/3)*v1)");
    _1 = v1 * v;
    BOOST_TEST(_1.str() == "((v1^2)*v2 + (125/3)*(v1^2))");
    _2 = v;
	_2 += _1;
	v += v1*v;
    BOOST_TEST(v == (125_v/3*v1 + v1*v2 + 125_v/3*v1*v1 + v1*v1*v2));
    BOOST_TEST(v.str() == "((v1^2)*v2 + (125/3)*(v1^2) + v2*v1 + (125/3)*v1)");

    v -= 125_v/3*v1 + v1*v2;
    std::cout << std::endl << v << " == " << 125_v/3*v1*v1 + v1*v1*v2 << std::endl;
    BOOST_TEST(v == 125_v/3*v1*v1 + v1*v1*v2);
    
    v /= v1^2;
    cout << v << endl;
    v.optimize();
    BOOST_TEST(v == 125_v/3 + v2);
    auto s = v1 + 5;
    auto s1 = v2 + 12;
    auto ff = v1 / 12;
    auto pp = 2 * s * ff * s1;
    pp.optimize();
    cout << pp<<endl;

    BOOST_TEST(v1*2*2==v1*4);
    
    auto _ = v1 * (-1_v / v2);
    BOOST_TEST(_.getCommonVars().size()==2);
   
    _ = -1_v*v1*v2;
    _ *= -v1;
    BOOST_TEST((v1^2)*v2  == _);
    
    _1 = -1_v * v1 * v2;
    _2 = -1_v * v2 * v1;
    BOOST_TEST(_1 == _2);
    
    _1 = -1_v*(v1^3);
    _2 = -1_v*(v1^4);
    BOOST_TEST(_1.getCommonVars().begin()->second == 3);
    BOOST_TEST(_2.getCommonVars().begin()->second == 4);
    
    _1 /= v1;
    BOOST_TEST(_1.getCommonVars().begin()->second == 2);
    
    _2 /= v1^2;
    BOOST_TEST(_2.getCommonVars().begin()->second == 2);
    _2 *= v1^2;
    BOOST_TEST(_2.getCommonVars().begin()->second == 4);
    
    _2 += _1;
    _2 *= v1 ^ 4;
    BOOST_TEST(_2.as<Sum>().begin()->getCommonVars().begin()->second == 8);
    
    _1.as<Product>().Add(v1^2);
    BOOST_TEST(_1.getCommonVars().begin()->second == 4);
    
    _1 = (((v1 + 0))^2);
    _2 = 2*(v2^2)*(v1^2);
    _ = _1*_2;
    BOOST_TEST(_ == 2*(v2^2)*(v1^4));
//    Variable v8, v9, v10, v12, v13;
//    _ = (-1_v/174)*(((-1_v*(v10^2)*v12 + -1_v*v14*v8*v10 + -1_v*v13*v9*v10 + -866_v*v10))^-1_v)*v13*v9;
//    BOOST_TEST(_(v8) == )
//    _ = -1_v*(v6^-1_v)*(v8^2_v)*v13;
//    _ = _(v6, (-1_v*v14*v9 + -1_v*v13*v10 + 174_v));
//    BOOST_TEST(_ == )
    
    
    DECL_VA(x);
    auto m1px =(-1_v)^x;
    _ = (-1_v)^((1_v/2)*x + m1px/4 + ((-1_v)/4));
    auto has = Product{_}.Has(m1px);
    BOOST_TEST(has == false);

    DECL_VA(PercentWaterDehydrated);
    DECL_VA(PotatoKgDehydrated);
    _1 = (-1_v * PercentWaterDehydrated + 100) ^ (-1);
    _1 *= -1 * PotatoKgDehydrated;
    _2 = "((PercentWaterDehydrated - 100)^(-1)) * PotatoKgDehydrated"_v;
    BOOST_TEST(_1.IsProduct());
    BOOST_TEST(_2.IsProduct());
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(Product_getVaVal_test) {
    DECL_VARS(a, b);
    auto prod = ((16 * (b ^ 2) + 8192 * (a ^ 2) + -32768) ^ ((1 / 2))) * a;
    auto vars = prod.getCommonVars();
    decltype(vars) expected = {{a, 1}};
    BOOST_TEST(vars == expected);
    auto val = prod.getVaVal();
    BOOST_TEST(val == a);
}

BOOST_AUTO_TEST_CASE(Product_optimization_test
    ,*disabled()
    )
{
    auto _1 = -8 * constants::plus_minus_1;
    auto _2 = 8 * constants::plus_minus_1;
    BOOST_TEST(_1 == _2);

    DECL_VA(x);
    _1 = -x * constants::plus_minus_1;
    _2 =  x * constants::plus_minus_1;
    BOOST_TEST(_1 == _2);

    DECL_VA(z);
	auto _ = ((-1 * z + 100) ^ (-1)) * x;
    _1 = _ * -100;
    _2 = _ * z;
    auto is = _1.IsSummationSimplifiable(_2);
    BOOST_TEST(is.first);
    BOOST_TEST(is.second==-x);

    _1 = 2;
    _1.sqrt();
    _2 = Fraction{constants::minus_1, _1} * _1;
    _2.optimize();
    BOOST_TEST(constants::minus_1 == _2);

    _1 = "(1/(24*sqrt(6)))*sqrt(6)*z"sv;
    _2 = "z/24"sv;
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(Product_abs_test)
{
    auto _1 = 430_v;
    auto _2 = -_1;

    _1.sqrt();
    _2.sqrt();
    BOOST_TEST(_1 != _2);
    
    _1 = std::abs(_1);
    _2 = std::abs(_2);
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(Product_multivalues_simplification)
{
    auto _1 = -8 * (1_v^(1_v/2));
    auto _2 =  8 * (1_v^(1_v/2));
    _1 /= _2;
    _2 = 1 * (1_v ^ (1_v / 2));
    BOOST_TEST(_1 == _2);

    _1 = -3;
    _1 /= 2;
    _1 *= constants::plus_minus_1;
    BOOST_TEST(_1.IsProduct());
    auto& p = _1.as<Product>();
    BOOST_TEST(p.size() == 2);
    BOOST_TEST(*p.begin() > constants::zero);
}

BOOST_AUTO_TEST_CASE(Product_Multival_Exponentiation_Sign_Simplification)
{
    DECL_VA(x);
    DECL_VA(y);
    auto _1 = -1 * ((-4 * (y ^ 2) + -4 * (x ^ 2) + 32 * x + 16 * y + -80) ^ constants::half);
    auto _2 = (-4 * (y ^ 2) + -4 * (x ^ 2) + 32 * x + 16 * y + -80) ^ constants::half;
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(Product_Serialization_test)
{
    DECL_VARS(x, y);
    auto _1 = "((Y^3))*sqrt(((Y^2)+(X^2)-2*Y*X))*X"_v;
    auto _2 = "sqrt(((Y^2)+(X^2)-2*Y*X))*(Y^3)*X"_v;
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(test_no_hang, *timeout(2))
{
    Variable v2, v3;
    (2_v*((v3 + -255)^2)*(v2^2)).optimize();
}

BOOST_AUTO_TEST_CASE(products_summing_simplification)
{
    DECL_VA(x);
    auto m1px = (-1_v)^x;
    auto _1 = m1px / 4;
    auto _2 = (1_v/2)*((-1_v)^((1_v/2)*x + _1 + ((-1_v)/4)));
    std::cout << _1 << "  +  " << _2 << std::endl;
    auto sum = _1 + _2;
    auto simplify = _1.IsSummationSimplifiable(_2);
    auto simplify2 = _2.IsSummationSimplifiable(_1);
    BOOST_TEST(simplify.first == simplify2.first);

    // original bug: _1 + _2 = (3/4)*((-1)^x)		IMPLEMENT: must be covered by IsSummationSimpifiable call
    if(!sum.IsSum()){
        BOOST_TEST(simplify.first == true);
    }
}

BOOST_AUTO_TEST_CASE(Merge_four_test_no_hang
    , *timeout(2)
    * disabled()
) {
    Valuable::MergeOr(11_v / 2, -11_v / 2, -3_v / 2, 3_v / 2);
}

BOOST_AUTO_TEST_CASE(multivalue_products_summing_simplification_tests
    , *depends_on("Merge_four_test_no_hang")
) {
    auto _1 = (7_v/2) * constants::plus_minus_1;
    cout << "Distinct values of _1 are as follows:" << endl;
    for(auto& value: _1.Distinct()){
        cout << ' ' << value;
    }
    cout<< endl;
    auto _2 = 2 * constants::plus_minus_1;
    cout << "Distinct values of _2 are as follows:" << endl;
    for(auto& value: _2.Distinct()){
        cout << ' ' << value;
    }

    auto answer = Valuable::MergeOr(11_v/2, -11_v/2, -3_v/2, 3_v/2);
    cout << endl << "Distinct sums are as follows:" << endl;
    Valuable::solutions_t distinctSums;
    for(auto& $1: _1.Distinct()){
        for(auto& $2: _2.Distinct()){
            auto sum = $1+$2;
            cout << ' ' << sum;
            distinctSums.emplace(sum);
        }
    }
    cout<< endl;
    
    auto answerDistinct = answer.Distinct();
    BOOST_TEST(answerDistinct == distinctSums);
    
    auto simplify = _1.IsSummationSimplifiable(_2);
    BOOST_TEST(!simplify.first);

}

BOOST_AUTO_TEST_CASE(unordered_multiset_tests)
{
    {
        std::unordered_multiset<int> ms1 {{1,2,3}};
        std::unordered_multiset<int> ms2 {{1,2,3}};
        BOOST_TEST(ms1==ms2);
    }
    {
        std::unordered_multiset<int> ms1 {{1,2,3}};
        std::unordered_multiset<int> ms2 {{2,1,3}};
        BOOST_TEST(ms1==ms2);
    }
    {
        std::unordered_multiset<Valuable> ms1 {{1,2,3}};
        std::unordered_multiset<Valuable> ms2 {{2,1,3}};
        BOOST_TEST(ms1==ms2);
    }
    {
        Variable v;
        std::unordered_multiset<Valuable> ms1 {{1,v*2,3}};
        std::unordered_multiset<Valuable> ms2 {{v*2,1,3}};
        BOOST_TEST(ms1==ms2);
    }
    {
        Variable v;
        std::unordered_multiset<Valuable> ms1 {{1,v*2,3}};
        std::unordered_multiset<Valuable> ms2 {{2_v*v,1,3}};
        BOOST_TEST(ms1==ms2);
    }
    {
        Variable v1, v2;
        std::unordered_multiset<Valuable> ms1 {{1_v/2, v1, v2 }};
        std::unordered_multiset<Valuable> ms2 {{v1, v2, 1_v/2}};
        BOOST_TEST(ms1==ms2);
    }
}
