/// <summary>
/// See for some reference and description on multivalues origins
/// </summary>
#define BOOST_TEST_MODULE Multivalues test
#include <boost/test/unit_test.hpp>

#include "Variable.h"


using namespace omnn::math;
using namespace boost::unit_test_framework;

std::string l(const omnn::math::Valuable& v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

BOOST_AUTO_TEST_CASE(MergeOr_tests)
{
#define ITEMS -1, 1
    auto o = Valuable::MergeOr(ITEMS);
    BOOST_TEST(constants::plus_minus_1 == o);
    auto d = o.Distinct();
    decltype(d) items = {ITEMS};
    BOOST_TEST(d == items);
#undef ITEMS

#define ITEMS -1, 1, 0
    o = Valuable::MergeOr(ITEMS); 
    d = o.Distinct();
    items = {ITEMS};
    BOOST_TEST(d == items);
#undef ITEMS

#define ITEMS -1, 1, 2, -2
    o = Valuable::MergeOr(ITEMS); 
    d = o.Distinct();
    items = {ITEMS};
    BOOST_TEST(d == items);
#undef ITEMS

}

BOOST_AUTO_TEST_CASE(Debranching_Dilemma_test)
{
    // Multivalue multiplication vs ^2 debranching dilemma
    auto plus_minus_two = constants::plus_minus_1 * constants::two; // ±2
    auto _1 = plus_minus_two; // ±2
    _1 *= constants::plus_minus_1; // still ±2
    BOOST_TEST(_1 == plus_minus_two);
    // same with x
    DECL_VA(x);
    auto _2 = x * 2; // 2x
    _2 *= x; // 2*(x^2)
    _2.Eval(x, constants::plus_minus_1); // not ±2
    BOOST_TEST(_2 == 2); // because x is the same for both branches
}

BOOST_AUTO_TEST_CASE(Other_signs_tests) {
    auto _1 = -1_v * (-4_v ^ constants::half) * (-16 ^ constants::half);
    auto _2 = constants::plus_minus_1 * 8 * constants::i;
    BOOST_TEST(_1 == _2);
    auto d1 = _1.Distinct();
}

BOOST_AUTO_TEST_CASE(Higher_orders_tests)
{
    auto _ = 841_v / 64; // 841/64 = 13.140625
    auto a = _.Sqrt();   // 29/8 = 3.625   - square root stands for principal root (only positive branch).
                         // Branch reduction leads to backward-incompatible expressions which leads to calculation
                         // misstakes which is why this framework has limited support for principal root
    BOOST_TEST(a == 3.625);

	// Multivalue expression equivalent:
    _ ^= 1_v/2; // (841/64)^(1/2) = ?.. lets see possible ways to calculate (covering multivals)
	// ±29/±8 = ±(29/8) = (1^(1/2))*(29/8) :   
	// 
	//  29^2  /  8^2  = 841/64
    //  29^2 / (-8)^2 = 841/64
    // (-29)^2 / 8^2  = 841/64
    // (-29)^2 / (-8)^2 = 841/64
	//                 <=>
	//	                  ___    - (841/64)^(1/2) equals some of these values and we do not know which of these exactly for current expression
	//  (841/64)^(1/2) = | 29/8 = -29/-8 = 3.625
	//                   | -29/8 = 29/-8 = -3.625
	//                   \___
	// 
	// NOTE: it does not equal to any of these particular values, but it equals this uncertinty, which of these two the value is
	//       probably the most suitable is to call it the set of these two values:
	//       (841/64)^(1/2) = ±29/±8 = ±(29/8) = set(-3.625, 3.625)
	//       mathematical unordered set of values
	//       multivalue
	//       this way there is not branch reduction and expressions are backward-compatibly equivalent
	//   ___                                  ___
    //  |  29^2  /  8^2  = 841/64            | (29/8)^2 = 841/64              ___
    //  |  29^2 / (-8)^2 = 841/64            | (29/-8)^2 = 841/64            | 29/8 = -29/-8 = 3.625
    //  | (-29)^2 / 8^2  = 841/64     <===>  | (-29/8)^2 = 841/64    <===> 	 | -29/8 = 29/-8 = -3.625    <===> (841/64)^(1/2) = ±29/±8 = ±(29/8) = set(-3.625, 3.625)
    //  | (-29)^2 / (-8)^2 = 841/64          | (-29/-8)^2 = 841/64			 \___
	//   \__                                  \__
	//
	//  (841/64)^(1/2) = ±(29/8)
	//
	// this framework emulates ± with multiplying by 1^(1/2) because it equals ±1 and ±1*x = ±x
	// 
    // Which makes  (841/64)^(1/2) = (29/8) * 1^(1/2)
    a = (29_v / 8) * (1_v ^ (1_v / 2));
    BOOST_TEST(_ == a);
	//  this may seamlesly be used in other expressions
	// 
	// Multivalue expressions are detectable, splittable and buildable
	//
	// NOTE: this kind of math makes possible arithmetics with unordered sets of values
	//

    a = (573440_v*(((841_v/64))^((1_v/2))) + 2115584)/262144;
    // (573440 * (±29/±8) + 2115584) / 262144
    a.optimize();
    auto ok = a.IsMultival() == Valuable::YesNoMaybe::Yes;
    BOOST_TEST(ok);
    
    for (auto i = 1; i < 8; ++i) {
        Valuable dimmensions(1<<i);
        auto multi = 1_v ^ (1_v / dimmensions);
        auto values = multi.Distinct();
        auto n = values.size();
        BOOST_TEST(n == dimmensions);

        _ = multi;
        _ /= _;
        BOOST_TEST(_ == multi);
    }
}
