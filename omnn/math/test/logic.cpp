#define BOOST_TEST_MODULE Logic test
#include <boost/test/unit_test.hpp>

#include "Variable.h"
#include "generic.hpp"


BOOST_AUTO_TEST_CASE(Equal_comparator_test) {
    auto Equal = X.Equals(Y);
    std::cout << "X=Y : " << Equal << std::endl;
    TestBooleanOperator(Equal, [](auto x, auto y) { return x == y; });
}

BOOST_AUTO_TEST_CASE(ABS_expression_test) {
    auto abs = X.Abs();
    std::cout << "abs(X) : " << abs << std::endl;
    TestXpression(abs, [](auto x) { return std::abs(x); });
}

BOOST_AUTO_TEST_CASE(Sign_expression_test, *disabled()) {
    auto Sign = X.Sign();
    std::cout << "sign(X) : " << Sign << std::endl;
    TestXpression(Sign, [](auto x) { return x ? x / std::abs(x) : x; });
}

BOOST_AUTO_TEST_CASE(logic_or_tests
    , *disabled()
) {
    DECL_VA(x);
    auto _1 = x.Equals(1) || x.Equals(2) || x.Equals(3);
    auto _2 = Valuable::Abet(x, {1, 2, 3});

    auto express1 = _1(x);
    auto express2 = _2(x);
    BOOST_TEST(express1 == express2);

    _2.optimize();
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(merge_or_tests
    , *disabled()
) {
    DECL_VA(x);
    auto _1 = x.Equals(1) || x.Equals(2) || x.Equals(3);
    auto express1 = _1(x);
    auto set1 = Valuable::MergeOr(1, 2, 3);
    BOOST_TEST(express1 == set1);
    auto set2 = Valuable({1_v, 2_v, 3_v});
    BOOST_TEST(set1 == set2);
}

BOOST_AUTO_TEST_CASE(Min_operator_test) {
    auto Minimum = X.Minimum(Y);
    std::cout << "Minimum(X,Y) : " << Minimum << std::endl;
    TestBinaryOperator(Minimum, [](auto x, auto y) { return std::min(x, y); });
}

BOOST_AUTO_TEST_CASE(LessOrEqual_comparator_test) {
    auto LessOrEqual = X.LessOrEqual(Y);
    std::cout << "LessOrEqual(X,Y) : " << LessOrEqual << std::endl;
    TestBooleanOperator(LessOrEqual, [](auto x, auto y) { return x <= y; });
}

BOOST_AUTO_TEST_CASE(IsNegativeThan_comparator_test) {
    auto IsNegativeThan = X.IsNegativeThan(Y);
    std::cout << "X<0, X=-Y : " << IsNegativeThan << std::endl;
    TestBooleanOperator(IsNegativeThan, [](auto x, auto y) { return x < 0 && x == -y; });
}

BOOST_AUTO_TEST_CASE(IsNegative_expression_test) {
    auto IsNegative = X.IsNegative();
    auto xIsPresent = IsNegative.FindVa() && *IsNegative.FindVa() == X;
    BOOST_TEST(xIsPresent);
    if (xIsPresent) {
        BOOST_TEST_MESSAGE("X < 0 : " << IsNegative);
        TestBooleanExpression(IsNegative, [](auto x) { return x < 0; });
    }
}

BOOST_AUTO_TEST_CASE(IsPositive_expression_test) {
    auto IsPositive = X.IsPositive();
    BOOST_TEST_MESSAGE("X>0 : " << IsPositive);
    TestBooleanExpression(IsPositive, [](auto x) { return x > 0; });
}

BOOST_AUTO_TEST_CASE(Less_comparator_test) {
    auto Less = X.Less(Y);
    std::cout << "X<Y : " << Less << std::endl;
    TestBooleanOperator(Less, [](auto x, auto y) { return x < y; });
}

BOOST_AUTO_TEST_CASE(NE_comparator_optimized_expression_test, *disabled()) {
    auto NE = X.NotEquals(Y);
    auto nz = !NE.IsZero();
    BOOST_TEST(nz);
    NE = NE.Optimized();
    nz = !NE.IsZero();
    BOOST_TEST(nz);
    BOOST_TEST_MESSAGE("X<>Y : " << NE);
}

BOOST_AUTO_TEST_CASE(NE_comparator_test) {
    auto NE = X.NotEquals(Y);
    std::cout << "X<>Y : " << NE << std::endl;
    TestBooleanOperator(NE, [](auto x, auto y) { return x != y; });
}

BOOST_AUTO_TEST_CASE(Sign_operator_test
                     , *disabled()
                     )
{
    DECL_VARS(X);
    auto SignOperatorExpression = X.Sign();
    std::cout << "X<0 : " << SignOperatorExpression << std::endl;
    for (auto x = 10; x--> -10;) {
        auto sign = std::signbit(x);
        auto negativeOperatorInstantiation = SignOperatorExpression;
        Valuable::vars_cont_t evalMap = {{X, x}};
        std::cout << '\n' << x << "<0 = ";
        negativeOperatorInstantiation.eval(evalMap);
        std::cout << " expression that must be equal to zero when true: " << negativeOperatorInstantiation
                  << std::endl;

        negativeOperatorInstantiation.optimize();
        std::cout << std::endl << "Is " << x << "<0 : " << negativeOperatorInstantiation << std::endl;
        bool b = {};
        auto boolLessOrEqualOp = negativeOperatorInstantiation.ToBool();
        BOOST_TEST(boolLessOrEqualOp == sign);
        boolLessOrEqualOp.eval(evalMap);
        BOOST_TEST(boolLessOrEqualOp == sign);
        if (boolLessOrEqualOp == true) {
            BOOST_TEST(boolLessOrEqualOp.IsInt());
            BOOST_TEST(negativeOperatorInstantiation.IsInt());
            b = negativeOperatorInstantiation.IsInt() && negativeOperatorInstantiation.ca() == 0;
            std::cout << std::endl << x << "<0 : " << b << std::endl;
        } else if (boolLessOrEqualOp == false) {
            BOOST_TEST(boolLessOrEqualOp.IsInt());
            b = negativeOperatorInstantiation == 0;
            std::cout << std::endl
                      << x << "<0 : " << b << ' ' << negativeOperatorInstantiation << " != 0" << std::endl;
        } else {
            std::cout << std::endl << x << "<0 : " << boolLessOrEqualOp << std::endl;
            BOOST_TEST(!"boolLessOp must have boolean value");
        }
        BOOST_TEST(boolLessOrEqualOp == b);

        auto ok = b == sign;
        if (!ok) {
            std::cout << "X=" << x << " " << ok << " bool: " << b << std::endl;
            BOOST_TEST(ok);
        }
    }
}

BOOST_AUTO_TEST_CASE(Negative_operator_test)
{
    auto NegativeOperatorExpression = X.IsNegative();
    std::cout << "X<0 : " << NegativeOperatorExpression << std::endl;
    for (auto x = 10; x--> -10;) {
        auto isLessEq = x < 0;
        auto negativeOperatorInstantiation = NegativeOperatorExpression;
        Valuable::vars_cont_t evalMap = {{X, x}};
        std::cout << '\n' << x << "<0 = ";
        negativeOperatorInstantiation.eval(evalMap);
        std::cout << " expression that must be equal to zero when true: " << negativeOperatorInstantiation
                  << std::endl;

        negativeOperatorInstantiation.optimize();
        std::cout << std::endl << "Is " << x << "<0 : " << negativeOperatorInstantiation << std::endl;
        bool b = {};
        auto boolLessOrEqualOp = negativeOperatorInstantiation.ToBool();
        BOOST_TEST(boolLessOrEqualOp == isLessEq);
        boolLessOrEqualOp.eval(evalMap);
        BOOST_TEST(boolLessOrEqualOp == isLessEq);
        if (boolLessOrEqualOp == true) {
            BOOST_TEST(boolLessOrEqualOp.IsInt());
            BOOST_TEST(negativeOperatorInstantiation.IsInt());
            b = negativeOperatorInstantiation.IsInt() && negativeOperatorInstantiation.ca() == 0;
            std::cout << std::endl << x << "<0 : " << b << std::endl;
        } else if (boolLessOrEqualOp == false) {
            BOOST_TEST(boolLessOrEqualOp.IsInt());
            b = negativeOperatorInstantiation == 0;
            std::cout << std::endl
                      << x << "<0 : " << b << ' ' << negativeOperatorInstantiation << " != 0" << std::endl;
        } else {
            std::cout << std::endl << x << "<0 : " << boolLessOrEqualOp << std::endl;
            BOOST_TEST(!"boolLessOp must have boolean value");
        }
        BOOST_TEST(boolLessOrEqualOp == b);

        auto ok = b == isLessEq;
        if (!ok) {
            std::cout << "X=" << x << " " << ok << " bool: " << b << std::endl;
            BOOST_TEST(ok);
        }
    }
}

BOOST_AUTO_TEST_CASE(NegativeOrZero_operator_test)
{
    auto NegativeOrZeroOperatorExpression = X.NegativeOrZero();
    std::cout << "X<=0 : " << NegativeOrZeroOperatorExpression << std::endl;
    for (auto x = 10; x--> -10;) {
        auto isLessEq = x <= 0;
        auto negativeOrZeroOperatorInstantiation = NegativeOrZeroOperatorExpression;
        Valuable::vars_cont_t evalMap = {{X, x}};
        std::cout << '\n' << x << "<=0 = ";
        negativeOrZeroOperatorInstantiation.eval(evalMap);
        std::cout << " expression that must be equal to zero when true: " << negativeOrZeroOperatorInstantiation
                  << std::endl;

        negativeOrZeroOperatorInstantiation.optimize();
        std::cout << std::endl << "Is " << x << "<=0 : " << negativeOrZeroOperatorInstantiation << std::endl;
        bool b = {};
        auto boolLessOrEqualOp = negativeOrZeroOperatorInstantiation.ToBool();
        BOOST_TEST(boolLessOrEqualOp == isLessEq);
        boolLessOrEqualOp.eval(evalMap);
        BOOST_TEST(boolLessOrEqualOp == isLessEq);
        if (boolLessOrEqualOp == true) {
            BOOST_TEST(boolLessOrEqualOp.IsInt());
            BOOST_TEST(negativeOrZeroOperatorInstantiation.IsInt());
            b = negativeOrZeroOperatorInstantiation.IsInt() && negativeOrZeroOperatorInstantiation.ca() == 0;
            std::cout << std::endl << x << "<=0 : " << b << std::endl;
        } else if (boolLessOrEqualOp == false) {
            BOOST_TEST(boolLessOrEqualOp.IsInt());
            b = negativeOrZeroOperatorInstantiation == 0;
            std::cout << std::endl
                      << x << "<=0 : " << b << ' ' << negativeOrZeroOperatorInstantiation << " != 0" << std::endl;
        } else {
            std::cout << std::endl << x << "<=0 : " << boolLessOrEqualOp << std::endl;
            BOOST_TEST(!"boolLessOp must have boolean value");
        }
        BOOST_TEST(boolLessOrEqualOp == b);

        auto ok = b == isLessEq;
        if (!ok) {
            std::cout << "X=" << x << " Y=0 " << ok << " bool: " << b << std::endl;
            BOOST_TEST(ok);
        }
    }
}

BOOST_AUTO_TEST_CASE(LessOrEqual_operator_test) {
    DECL_VARS(X, Y);
    auto LE = X.LessOrEqual(Y);
    auto lessOrEqual = LE.CompiLambda(X, Y);
    std::cout << "X<=Y : " << LE << std::endl;
    for (auto x = 10; x--> -10;) {
        for (auto y = 10; y--> -10;) {
            auto isLessEq = x <= y;
            auto lessOrEqualOperatorInstantiation = LE;
            Valuable::vars_cont_t evalMap = {{X, x}, {Y, y}};
            std::cout << '\n' << x << "<=" << y << " = ";
            lessOrEqualOperatorInstantiation.eval(evalMap);
            std::cout << " expression that must be equal to zero when true: " << lessOrEqualOperatorInstantiation
                      << std::endl;

            lessOrEqualOperatorInstantiation.optimize();
            std::cout << std::endl << "Is " << x << "<=" << y << " : " << lessOrEqualOperatorInstantiation << std::endl;
            bool b = {};
            auto boolLessOrEqualOp = lessOrEqualOperatorInstantiation.ToBool();
            BOOST_TEST(boolLessOrEqualOp == isLessEq);
            boolLessOrEqualOp.eval(evalMap);
            BOOST_TEST(boolLessOrEqualOp == isLessEq);
            if (boolLessOrEqualOp == true) {
                BOOST_TEST(boolLessOrEqualOp.IsInt());
                BOOST_TEST(lessOrEqualOperatorInstantiation.IsInt());
                b = lessOrEqualOperatorInstantiation.IsInt() && lessOrEqualOperatorInstantiation.ca() == 0;
                std::cout << std::endl << x << "<=" << y << " : " << b << std::endl;
            } else if (boolLessOrEqualOp == false) {
                BOOST_TEST(boolLessOrEqualOp.IsInt());
                b = lessOrEqualOperatorInstantiation == 0;
                std::cout << std::endl
                          << x << "<=" << y << " : " << b << ' ' << lessOrEqualOperatorInstantiation << " != 0" << std::endl;
            } else {
            	std::cout << std::endl << x << "<=" << y << " : " << boolLessOrEqualOp << std::endl;
                BOOST_TEST(!"boolLessOp must have boolean value");
            }
            BOOST_TEST(boolLessOrEqualOp == b);

            auto ok = b == isLessEq;
            if (!ok) {
                std::cout << "X=" << x << " Y=" << y << ' ' << ok << " bool: " << b << std::endl;
                BOOST_TEST(ok);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(Delta_function_test
	, *disabled() // FIXME:
) { // https://en.wikipedia.org/wiki/Dirac_delta_function
    auto LE = X.LessOrEqual(Y);
    auto deltaFunction_bool = LE.ToBool();
    std::cout << "X<=Y : " << LE << std::endl;
    std::cout << "bool(X<=Y) : " << deltaFunction_bool << std::endl;
    for (auto x = 10; x-- > -10;) {
        for (auto y = 10; y-- > -10;) {
            auto isLessEq = x <= y;
            auto lessOrEqualOperatorInstantiation = deltaFunction_bool;
            Valuable::vars_cont_t evalMap = {{X, x}, {Y, y}};
            bool b = {};
            auto boolLessOrEqualOp = deltaFunction_bool;
            boolLessOrEqualOp.eval(evalMap);
            BOOST_TEST(boolLessOrEqualOp == isLessEq);
            if (boolLessOrEqualOp == true) {
            } else if (boolLessOrEqualOp == false) {
            } else {
                std::cout << std::endl << x << "<=" << y << " : " << boolLessOrEqualOp << std::endl;
                BOOST_TEST(!"boolLessOp must have boolean value");
            }
            BOOST_TEST(boolLessOrEqualOp == b);

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

BOOST_AUTO_TEST_CASE(Less_operator_test) {
    auto Less = X.Less(Y);
    std::cout << "X<Y : " << Less << std::endl;
    for (auto x = 10; x--> -10;) {
        for (auto y = 10; y--> -10;) {
            auto xIsLess = x < y;
            auto lessOperatorInstantiation = Less;
            Valuable::vars_cont_t evalMap = {{X, x}, {Y, y}};
            std::cout << '\n' << x << "<" << y << " = ";
            lessOperatorInstantiation.eval(evalMap);
            std::cout << " expression that must be equal to zero when true: " << lessOperatorInstantiation
                      << std::endl;

            lessOperatorInstantiation.optimize();
            std::cout << std::endl << "Is " << x << "<" << y << " : " << lessOperatorInstantiation << std::endl;
            bool b = {};
            auto boolLessOp = lessOperatorInstantiation.ToBool();
            BOOST_TEST(boolLessOp == xIsLess);
            if (boolLessOp == true) {
                BOOST_TEST(boolLessOp.IsInt());
                BOOST_TEST(lessOperatorInstantiation.IsInt());
                b = lessOperatorInstantiation.IsInt() && lessOperatorInstantiation.ca() == 0;
                std::cout << std::endl << x << "<" << y << " : " << b << std::endl;
            } else if (boolLessOp == false) {
                BOOST_TEST(boolLessOp.IsInt());
                b = lessOperatorInstantiation == 0;
                std::cout << std::endl
                          << x << "<" << y << " : " << b << ' ' << lessOperatorInstantiation << " != 0"
                          << std::endl;
            } else {
                std::cout << std::endl << x << "<" << y << " : " << boolLessOp << std::endl;
                BOOST_TEST(!"boolLessOp must have boolean value");
            }
            BOOST_TEST(boolLessOp == b);

            auto ok = b == xIsLess;
            if (!ok) {
                std::cout << "X=" << x << " Y=" << y << ' ' << ok << " bool: " << b << std::endl;
                BOOST_TEST(ok);
            }
        }
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
