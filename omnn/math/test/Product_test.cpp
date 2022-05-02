#define BOOST_TEST_MODULE Product test
#include <boost/test/unit_test.hpp>

#include "Product.h"
#include "Sum.h"
#include "Fraction.h"
#include "Variable.h"

using namespace std;
using namespace omnn::math;
using namespace boost::unit_test;

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

	_ = ((-1 * v1 + 100) ^ (-1)) * v2;
    _1 = _ * -100;
    _2 = _ * v1;
    auto is = _1.IsSummationSimplifiable(_2);
    BOOST_TEST(is.first);
    BOOST_TEST(is.second==-v2);
}

BOOST_AUTO_TEST_CASE(Product_optimization_test)
{
    auto _1 = -8 * (1_v^(1_v/2));
    auto _2 =  8 * (1_v^(1_v/2));
    BOOST_TEST(_1 == _2);

    DECL_VA(x);
    _1 = -x * constants::plus_minus_1;
    _2 =  x * constants::plus_minus_1;
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(Product_multivalues_simplification)
{
    auto _1 = -8 * (1_v^(1_v/2));
    auto _2 =  8 * (1_v^(1_v/2));
    _1 /= _2;
    _2 = 1 * (1_v ^ (1_v / 2));
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

    _1 = (7_v/2) * constants::plus_minus_1;
    cout << "Distinct values of _1 are as follows:" << endl;
    for(auto& value: _1.Distinct()){
        cout << ' ' << value;
    }
    cout<< endl;
    _2 = 2 * constants::plus_minus_1;
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
    
    simplify = _1.IsSummationSimplifiable(_2);
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
