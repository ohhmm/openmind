#define BOOST_TEST_MODULE Logic test
#include <boost/test/unit_test.hpp>

#include "Variable.h"

using namespace omnn::math;
using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(logic_or_tests)
{
    auto x = "x"_va;
    auto eq = x.Equals(1).logic_or(x.Equals(2)).logic_or(x.Equals(3));
    auto ok = eq(x);
    //TODO: BOOST_TEST(ok ==
}

BOOST_AUTO_TEST_CASE(ifz_tests)
{
    // two different bits
    // if a=0 then b=1 else b=0
    // if a=1 then b=0 else b=1
    Variable a,b;
    auto e = a.Equals(1).Ifz(b.Equals(0), b.Equals(1));
    {
        auto ee = e;
        ee.Eval(a, 0);
        ee.Eval(b, 1);
        ee.optimize();
        BOOST_TEST(ee==0);
    }
    {
        auto ee = e;
        ee.Eval(a, 1);
        ee.Eval(b, 0);
        ee.optimize();
        BOOST_TEST(ee==0);
    }
}

BOOST_AUTO_TEST_CASE(not_tests
    ,*disabled() //TODO:
)
{
    auto x = "x"_va;
    auto x_eq_1 = x-1;
    auto x_ne_1 = !x_eq_1; //!x; // must mean all except this equation
    std::cout << x_ne_1 << std::endl;
    auto eval_x_eq_1 = x_eq_1;
    eval_x_eq_1.Eval(x, 1);
    auto ok = eval_x_eq_1 == 0_v;
    BOOST_TEST(ok);

    auto eval_x_ne_1 = x_ne_1;
    eval_x_ne_1.Eval(x, 7);
    ok = eval_x_ne_1 != 0_v;
    BOOST_TEST(ok);
}

BOOST_AUTO_TEST_CASE(test_logic_intersection)
{
    Variable x;
    auto _1 = x.Abet(x, {1,2,3,3});
    auto _2 = x.Abet(x, {2,3,3});
    auto _ = _1.Intersect(_2, x);

    auto solutions = _.IntSolutions(x);
    if(solutions.size() != 2)
		for (auto& s : solutions){
			std::cout << s << std::endl;
		}
    decltype(solutions) check = { 2, 3 };
    BOOST_TEST(solutions == check);
}

BOOST_AUTO_TEST_CASE(test_logic_intersection_with_exception
    ,*disabled() //TODO:
)
{
    Variable x;
    auto _1 = x.Abet(x, {1,2,3,3});
    auto _2 = x.Abet(x, {2,3,3});
    auto _ = _1.Intersect(_2, x).logic_and(x.NotEquals(3));
    
    auto solutions = _.IntSolutions(x);
    if(solutions.size() != 1)
		for (auto& s : solutions){
			std::cout << s << std::endl;
		}
    BOOST_TEST(solutions.size() == 1);
    if(solutions.size()){
        _ = *solutions.begin();
        BOOST_TEST(_ == 2);
    }
}

#include "Sum.h"

BOOST_AUTO_TEST_CASE(test_logic_intersection_simplifying
                      ,*disabled()
                     )
{
    Variable x;
    auto _1 = x.Abet(x, {1,2});
    auto _2 = x.Abet(x, {2,3});
    auto i = _1.Intersect(_2, x);
    auto solutions = i.IntSolutions(x);
    BOOST_TEST(solutions.size() == 1);
    auto _ = *solutions.begin();
    BOOST_TEST(_ == 2);
    BOOST_TEST(i.IsSum());
    auto& sum = i.as<Sum>();
    std::vector<Valuable> coefficients;
    auto grade = sum.FillPolyCoeff(coefficients, x);
    BOOST_TEST(grade == 1);
}
