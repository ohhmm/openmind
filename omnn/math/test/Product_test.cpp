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
    
    Variable v1, v2;
    auto p1 = 1_v/2*v1*v2;
    auto p2 = v1*v2*1_v/2;
    BOOST_TEST(Product::cast(p1));
    BOOST_TEST(Product::cast(p2));
    BOOST_TEST(p1 == p2);
    p1.Eval(v1, 3);
    p1.Eval(v2, 4);
    p1.optimize();
    BOOST_TEST(p1 == 6);
    v *= v1;
    v += v1*v2;
    BOOST_TEST(v == (125_v/3*v1 + v1*v2));
    cout<<v;
    v += v1*v;
    BOOST_TEST(v == (125_v/3*v1 + v1*v2 + 125_v/3*v1*v1 + v1*v1*v2));

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
    
    auto _1 = -1_v*v1*v2;
    auto _2 = -1_v*v2*v1;
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
    BOOST_TEST(Sum::cast(_2)->begin()->getCommonVars().begin()->second == 8);
    
    Product::cast(_1)->Add(v1^2);
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
}

BOOST_AUTO_TEST_CASE(test_no_hang, *timeout(2))
{
    Variable v2, v3;
    (2_v*((v3 + -255)^2)*(v2^2)).optimize();
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
