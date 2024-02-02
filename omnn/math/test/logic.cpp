#define BOOST_TEST_MODULE Logic test
#include <boost/test/unit_test.hpp>

#include "Variable.h"

using namespace omnn::math;
using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(logic_or_tests
    , *disabled() // FIXME:
) {
    DECL_VA(x);
    auto eq = x.Equals(1).logic_or(x.Equals(2)).logic_or(x.Equals(3));
    auto ok = eq(x);
    auto set = Valuable({1_v, 2_v, 3_v});
    BOOST_TEST(ok == set);
}

BOOST_AUTO_TEST_CASE(LessOrEqual_operator_test) {
    DECL_VARS(X, Y);
    auto Minimum = X.Min(Y);
    std::cout << "Minimum(X,Y) : " << Minimum << std::endl;
    auto LE = X.LessOrEqual(Y);
    std::cout << "X<=Y : " << LE << std::endl;
    for (auto x = 10; x-- > 1;) {
        for (auto y = 10; y-- > 1;) {
            auto isLessEq = x <= y;
            auto minOfTwo = std::min(x, y);
            auto minimum = Minimum;
            auto lessOrEqualOperatorInstantiation = LE;
            Valuable::vars_cont_t evalMap = {{X, x}, {Y, y}};
            minimum.eval(evalMap);
            BOOST_TEST(minimum == minOfTwo);
            std::cout << '\n' << x << "<=" << y << " = ";
            lessOrEqualOperatorInstantiation.eval(evalMap);
            std::cout << " expression that must be equal to zero when true: " << lessOrEqualOperatorInstantiation
                      << std::endl;

            lessOrEqualOperatorInstantiation.optimize();
            std::cout << std::endl << "Is " << x << "<=" << y << " : " << lessOrEqualOperatorInstantiation << std::endl;
            bool b = {};
            auto boolLessOp = lessOrEqualOperatorInstantiation.ToBool();
            BOOST_TEST(boolLessOp == isLessEq);
            if (boolLessOp == true) {
                BOOST_TEST(boolLessOp.IsInt());
                BOOST_TEST(lessOrEqualOperatorInstantiation.IsInt());
                b = lessOrEqualOperatorInstantiation.IsInt() && lessOrEqualOperatorInstantiation.ca() == 0;
                std::cout << std::endl << x << "<=" << y << " : " << b << std::endl;
            } else if (boolLessOp == false) {
                BOOST_TEST(boolLessOp.IsInt());
                b = lessOrEqualOperatorInstantiation == 0;
                std::cout << std::endl
                          << x << "<=" << y << " : " << b << ' ' << lessOrEqualOperatorInstantiation << " != 0" << std::endl;
            } else {
            	std::cout << std::endl << x << "<=" << y << " : " << boolLessOp << std::endl;
                BOOST_TEST(!"boolLessOp must have boolean value");
            }
            BOOST_TEST(boolLessOp == b);

            auto ok = b == isLessEq;
            if (!ok) {
                std::cout << "X=" << x << " Y=" << y << ' ' << ok << " bool: " << b << std::endl;
                BOOST_TEST(ok);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(ifz_tests) {
    // two different bits
    // if a=0 then b=1 else b=0
    // if a=1 then b=0 else b=1
    DECL_VA(a);
    DECL_VA(b);
    auto e = a.Equals(1).Ifz(b.Equals(0), b.Equals(1));
    {
        auto ee = e;
        ee.Eval(a, 0);
        ee.Eval(b, 1);
        ee.optimize();
        BOOST_TEST(ee == 0);
    }
    {
        auto ee = e;
        ee.Eval(a, 1);
        ee.Eval(b, 0);
        ee.optimize();
        BOOST_TEST(ee == 0);
    }
}

BOOST_AUTO_TEST_CASE(not_tests
    ,*disabled() //TODO:
)
{
    DECL_VA(x);
    auto x_eq_1 = x - 1;
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

    auto eq = x.Equals(1).logic_or(x.Equals(2)).logic_or(x.Equals(3));
    auto neq = x_ne_1;
    auto intersection = eq && neq;
    auto _2or3 = x.Equals(2) || x.Equals(3);
    ok = intersection == _2or3;
    BOOST_TEST(ok);
}

BOOST_AUTO_TEST_CASE(test_logic_intersection
    , *disabled() // FIXME:
) {
    DECL_VA(x);
    auto _1 = x.Abet({1, 2, 3, 3});
    auto _2 = x.Abet({2,3,3});
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
    DECL_VA(x);
    auto _1 = x.Abet({1,2,3,3});
    auto _2 = x.Abet({2,3,3});
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
    DECL_VA(x);
    auto _1 = x.Abet({1, 2});
    auto _2 = x.Abet({2,3});
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
