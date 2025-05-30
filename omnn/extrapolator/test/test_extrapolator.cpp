//
// Created by Сергей Кривонос on 10.08.17.
//
#define BOOST_TEST_MODULE Extrapolator test
#include <boost/test/unit_test.hpp>
#include "Extrapolator.h"
#include "omnn/math/Sum.h"

using namespace omnn::math;
using namespace boost::unit_test;


std::string l(const omnn::math::Valuable& v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

BOOST_AUTO_TEST_CASE(ExtrapolatorSolve_test)
{
    
}

BOOST_AUTO_TEST_CASE(Extrapolator_test
    , *disabled()
)
{
    // lets define math vars:
    // TODO : link to youtube video with description of the sample for deep learning
    // for example we'll take bool square 2x2
    // lets say the goal is to determine type of squere
    // verticals:   {1,0},{1,0} or {0,1},{0,1}
    // horizontals: {1,1},{0,0} or {0,0},{1,1}
    // diagonals:   {1,0},{0,1} or {0,1},{1,0}
    // this way the vars are as follows:
    // box four vars a,b,c,d;  quantors: is_vericals, is_horizontals, is_diagonals
    
    //    Extrapolator<> e_verticals {{ {1,0,
    //                                   1,0,      1},
    //
    //                                  {0,1,
    //                                   0,1,      1},
    //
    //                                        {0,0,0,0,0},
    //                                        {1,1,1,1,0},
    //                                        {0,1,1,0,0}
    //                      }};
    
    // Test vericals:
    Extrapolator e_verticals {{
        {1,-1,
            1,-1},
        
        {-1,1,
            -1,1},
        
                {-1,-1,-1,-1},
                {1,1,1,1}
    }};
    
    ublas::vector<Extrapolator::value_type> augment(e_verticals.size1());
    augment[0] = 2;
    augment[1] = 1;
    //    augment[2] = -1;
    //    augment[3] = -2;
    //BOOST_TEST(e_verticals.Consistent(augment));
    
    auto r = e_verticals.Solve(augment);
    std::cout << r[0] << ' ' << r[1] << ' ' << r[2] << ' ' << r[3] << std::endl;
    auto val0 = r[0] * 1 + r[1] * -1 + r[2] * 1 + r[3] * -1;
    std::cout << "(r[0] * 1 + r[1] * -1 + r[2] * 1 + r[3] * -1) is " << val0 << std::endl;
    BOOST_TEST(r[0] * 1 + r[1] * -1 + r[2] * 1 + r[3] * -1 == 1.);
    auto val1 = r[0] * -1 + r[1] * 1 + r[2] * -1 + r[3] * 1;
    std::cout << "(r[0] * -1 + r[1] * 1 + r[2] * -1 + r[3] * 1) is " << val1 << std::endl;
    BOOST_TEST(r[0] * -1 + r[1] * 1 + r[2] * -1 + r[3] * 1 == 2);
    auto val2 = r[0] * -1 + r[1] * -1 + r[2] * -1 + r[3] * -1;
    std::cout << "(r[0] * 1 + r[1] * 1 + r[2] * 1 + r[3] * 1) is " << val2 << std::endl;
    BOOST_TEST(r[0] * -1 + r[1] * -1 + r[2] * -1 + r[3] * -1 == -1);
    auto val3 = r[0] * 1 + r[1] * 1 + r[2] * 1 + r[3] * 1;
    std::cout << "(r[0] * 1 + r[1] * 1 + r[2] * 1 + r[3] * 1) is " << val3 << std::endl;
    BOOST_TEST(r[0] * 1 + r[1] * 1 + r[2] * 1 + r[3] * 1 == -2);
    
    Extrapolator e {{
        {0,0,0,0, 0,0,0},
        // verticals
        {1,0,1,0, 1,0,0},
        {0,1,0,1, 1,0,0},
        // horizontals
        {1,1,0,0, 0,1,0},
        {0,0,1,1, 0,1,0},
        // diagonals
        {0,1,1,0, 0,0,1},
        {1,0,0,1, 0,0,1},
    }};
    BOOST_CHECK_NO_THROW(e.Determinant());
    
    //BOOST_TEST(e.Consistent());
}

BOOST_AUTO_TEST_CASE(ViewMatrix_test)
{
    BOOST_TEST_MESSAGE("Creating initial Extrapolator matrix");
    Extrapolator e {{ {1, 2},
        {3, 4} }};
    {
        BOOST_TEST_MESSAGE("Converting Extrapolator to Valuable");
        Valuable v = e;
        BOOST_TEST_MESSAGE("Valuable representation: " << v);
        std::cout << v << std::endl;
    }

    // view matrix
    //    0 0 1
    //    0 1 2
    //    1 0 3
    //    1 1 4
    BOOST_TEST_MESSAGE("Creating view matrix");
    auto vm = e.ViewMatrix();
    BOOST_TEST_MESSAGE("View matrix created with dimensions: " << vm.size1() << "x" << vm.size2());
    BOOST_TEST(vm.size1() == 4);
    BOOST_TEST(vm.size2() == 3);
    BOOST_TEST_MESSAGE("Testing view matrix elements");
    BOOST_TEST(vm(0,0) == 0); BOOST_TEST(vm(0,1) == 0); BOOST_TEST(vm(0,2) == 1);
    BOOST_TEST(vm(1,0) == 0); BOOST_TEST(vm(1,1) == 1); BOOST_TEST(vm(1,2) == 2);
    BOOST_TEST(vm(2,0) == 1); BOOST_TEST(vm(2,1) == 0); BOOST_TEST(vm(2,2) == 3);
    BOOST_TEST(vm(3,0) == 1); BOOST_TEST(vm(3,1) == 1); BOOST_TEST(vm(3,2) == 4);
    {
        // ax+by+cz=0
        // a=0:N, b=0:M, c=-(ax+by)/z     // if x=0 and y=0 then c becames 0 but it wasnt;
        // lets make c a va
        BOOST_TEST_MESSAGE("Creating equation system");
        Valuable eq = 1_v;
        DECL_VA(x);
        DECL_VA(y);
        DECL_VA(z);
        for (auto i=vm.size1(); i--;) {
            BOOST_TEST_MESSAGE("Processing row " << i);
            auto e1 = x - vm(i,0);
            auto e2 = y - vm(i,1);
            auto e3 = z - vm(i,2);
            BOOST_TEST_MESSAGE("Created expressions: e1=" << e1 << ", e2=" << e2 << ", e3=" << e3);
            auto subsyst = e1*e1 + e2*e2 + e3*e3; // squares sum equivalent to conjunction
            BOOST_TEST_MESSAGE("Subsystem: " << subsyst);
            std::cout << subsyst << std::endl;

            eq *= subsyst;
            BOOST_TEST_MESSAGE("Updated equation: " << eq);

            BOOST_TEST(subsyst.IsSum());
            BOOST_TEST_MESSAGE("Creating formula for subsystem");
            auto formula = FormulaOfVaWithSingleIntegerRoot(z, subsyst.as<Sum>());
            BOOST_TEST_MESSAGE("Formula created: " << formula);
            std::cout << "formula of value: " << formula << std::endl;
            auto evaluated = formula(vm(i, 0), vm(i, 1));
            BOOST_TEST_MESSAGE("Evaluated formula: " << evaluated);
            std::cout << evaluated << std::endl;
            evaluated.optimize();
            BOOST_TEST_MESSAGE("Optimized result: " << evaluated);
            std::cout << evaluated << std::endl;
            BOOST_TEST(evaluated == vm(i, 2)); // test row formula

            BOOST_TEST_MESSAGE("Evaluating subsystem");
            subsyst.Eval(x, vm(i, 0));
            subsyst.Eval(y, vm(i, 1));
            subsyst.Eval(z, vm(i, 2));
            subsyst.optimize();
            BOOST_TEST(subsyst == 0); // test row equation

            BOOST_TEST_MESSAGE("Testing current equation");
            auto e = eq;
            e.Eval(x, vm(i, 0));
            e.Eval(y, vm(i, 1));
            e.Eval(z, vm(i, 2));
            e.optimize();
//            std::cout << e.str() << std::endl;
            BOOST_TEST(e == 0); //test current eq
        }
        std::cout << "Total equation:" << eq << std::endl;
        BOOST_TEST(eq.IsSum());
        BOOST_TEST_MESSAGE("Creating final formula");
        auto f = FormulaOfVaWithSingleIntegerRoot(z, eq.as<Sum>());
        std::cout << "Formula : " << f << std::endl;
        // checking
        BOOST_TEST_MESSAGE("Verifying formula for all points");
        for (auto i=vm.size1(); i--;) {
            Valuable v = eq;
            v.Eval(x, vm(i,0));
            v.Eval(y, vm(i,1));
            v.Eval(z, vm(i,2));
            v.optimize();
            BOOST_TEST(v == 0);  //test whole equation
//            std::cout << std::endl << vm(i,2) << " : " << v << std::endl;
            BOOST_TEST(f(vm(i,0),vm(i,1))==vm(i,2)); // test formula
        }
    }
    BOOST_TEST_MESSAGE("Creating test matrix m");
    Extrapolator m {{
        {0,0},
        {0,1},
        {1,0},
        {1,1}
    }};
    ublas::vector<Valuable> au(4);
    for(int i=1; i<4; ++i)
    {
        au[i] = i;
    }
}

#if defined(WIN32) || defined(__APPLE__)
#define CODEC_TEST_PLATFORM_SPECIFIC_ENABLE
#else
#define CODEC_TEST_PLATFORM_SPECIFIC_ENABLE ,*disabled()
#endif

BOOST_AUTO_TEST_CASE(Codec_test
    CODEC_TEST_PLATFORM_SPECIFIC_ENABLE
)
{
    Extrapolator ex {{
        //  input   output
        {0,0,0,0, 0,0,0},
        // verticals
        {1,0,1,0, 1,0,0},
        {0,1,0,1, 1,0,0},
        // horizontals
        {1,1,0,0, 0,1,0},
        {0,0,1,1, 0,1,0},
        // diagonals
        {0,1,1,0, 0,0,1},
        {1,0,0,1, 0,0,1},
    }};

//    Extrapolator e {{       // input    output
//        {0,0},              //{0,0,0,0, 0,0,0},
//        // verticals
//        {10,4},             //{1,0,1,0, 1,0,0},
//        {5,4},              //{0,1,0,1, 1,0,0},
//        // horizontals
//        {12,2},             //{1,1,0,0, 0,1,0},
//        {3,2},              //{0,0,1,1, 0,1,0},
//        // diagonals
//        {6,1},              //{0,1,1,0, 0,0,1},
//        {9,1},              //{1,0,0,1, 0,0,1},
//    }};
    
    Variable x,y,z;
    auto f = ex.Factors(y,x,z);
    std::list<Variable> formulaParamSequence = {y,x};
    FormulaOfVaWithSingleIntegerRoot fo(z, f, &formulaParamSequence);
    
    // TODO : extrapolation
//    std::cout << fo(ex.size1(), ex.size2()) << std::endl;

    // inbound data deduce
    for (auto i=ex.size1(); i--;) { // raw
        for (auto j=ex.size2(); j--;) { // column
            auto c = f;
            c.Eval(x, j);
            c.Eval(y, i);
            c.Eval(z, ex(i,j));
            c.optimize();
            BOOST_TEST(c==0_v);//
            
            c = fo(i,j);
            std::cout << c.str() << std::endl;
            BOOST_TEST(c == ex(i,j));
        }
    }
}
