//
// Created by Сергей Кривонос on 10.08.17.
//
#define BOOST_TEST_MODULE Extrapolator test
#include <boost/test/unit_test.hpp>
#include "Extrapolator.h"

using namespace omnn::extrapolator;
using namespace boost::unit_test;


std::string l(const omnn::extrapolator::Valuable& v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

BOOST_AUTO_TEST_CASE(ExtrapolatorSolve_test)
{
    
}

BOOST_AUTO_TEST_CASE(Extrapolator_test, *disabled())
{
    // lets define extrapolator vars:
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
        
//        {-1,-1,-1,-1},
//        {1,1,1,1}
    }};

    ublas::vector<double> augment(e_verticals.size1());
    augment[0] = 2;
    augment[1] = 1;
//    augment[2] = -1;
//    augment[3] = -2;
    //BOOST_TEST(e_verticals.Consistent(augment));

    auto r = e_verticals.Solve(augment);
    std::cout << r[0] << ' ' << r[1] << ' ' << r[2] << ' ' << r[3] << std::endl;
    auto val = r[0] * 1 + r[1] * -1 + r[2] * 1 + r[3] * -1;
    std::cout << "(r[0] * 1 + r[1] * -1 + r[2] * 1 + r[3] * -1) is " << val << std::endl;
    BOOST_TEST(r[0] * 1 + r[1] * -1 + r[2] * 1 + r[3] * -1 == 1.);
    val = r[0] * -1 + r[1] * 1 + r[2] * -1 + r[3] * 1;
    std::cout << "(r[0] * -1 + r[1] * 1 + r[2] * -1 + r[3] * 1) is " << val << std::endl;
    BOOST_TEST(r[0] * -1 + r[1] * 1 + r[2] * -1 + r[3] * 1 == 2);
    val = r[0] * -1 + r[1] * -1 + r[2] * -1 + r[3] * -1;
    std::cout << "(r[0] * 1 + r[1] * 1 + r[2] * 1 + r[3] * 1) is " << val << std::endl;
    BOOST_TEST(r[0] * -1 + r[1] * -1 + r[2] * -1 + r[3] * -1 == -1);
    val = r[0] * 1 + r[1] * 1 + r[2] * 1 + r[3] * 1;
    std::cout << "(r[0] * 1 + r[1] * 1 + r[2] * 1 + r[3] * 1) is " << val << std::endl;
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
    Extrapolator e {{ {1, 2},
                      {3, 4} }};
    {
    Valuable v = e;
    std::cout << v << std::endl;
    }
    
// view matrix
//    0 0 1
//    0 1 2
//    1 0 3
//    1 1 4
    auto vm = e.ViewMatrix();
    BOOST_TEST(vm.size1() == 4);
    BOOST_TEST(vm.size2() == 3);
    BOOST_TEST(vm(0,0) == 0); BOOST_TEST(vm(0,1) == 0); BOOST_TEST(vm(0,2) == 1);
    BOOST_TEST(vm(1,0) == 0); BOOST_TEST(vm(1,1) == 1); BOOST_TEST(vm(1,2) == 2);
    BOOST_TEST(vm(2,0) == 1); BOOST_TEST(vm(2,1) == 0); BOOST_TEST(vm(2,2) == 3);
    BOOST_TEST(vm(3,0) == 1); BOOST_TEST(vm(3,1) == 1); BOOST_TEST(vm(3,2) == 4);
    {
    // ax+by+cz=0
    // a=0:N, b=0:M, c=-(ax+by)/z     // if x=0 and y=0 then c becames 0 but it wasnt;
    // lets make c a va
    Valuable eq = 1_v;
    Variable x,y,z;
    for (auto i=vm.size1(); i--;) {
        auto e1 = x - vm(i,0);
        auto e2 = y - vm(i,1);
        auto e3 = z - vm(i,2);
        auto subsyst = e1*e1 + e2*e2 + e3*e3; // squares sum equivalent to conjunction
        std::cout << subsyst << std::endl;
        eq += subsyst*subsyst;
        auto sum = Sum::cast(subsyst);
        BOOST_TEST(sum);
        auto formula = sum->FormulaOfVa(z);
        std::cout << "formula of value: " << formula << std::endl;
        auto evaluated = formula(vm(i, 0), vm(i, 1));
        std::cout << evaluated << std::endl;
        evaluated.optimize();
        std::cout << evaluated << std::endl;
        BOOST_TEST(evaluated == vm(i, 2));
        subsyst.Eval(x, vm(i, 0));
        subsyst.Eval(y, vm(i, 1));
        subsyst.Eval(z, vm(i, 2));
        BOOST_TEST(subsyst == 0);
    }
    std::cout << eq << std::endl;
    // checking
    for (auto i=vm.size1(); i--;) {
        Valuable v = eq;
        v.Eval(x, vm(i,0));
        v.Eval(y, vm(i,1));
        v.optimize();
        std::cout << std::endl << vm(i,2) << " : " << v << std::endl;
    }
    }
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

    // this seems not work
//    auto s = m.Solve(au);
//    for(auto si : s)
//    {
//        std::cout << si << ' ';
//    }
//    std::cout << std::endl;
//
//    // check this solution
//    for (auto i = m.size1(); i--; ) {
//        Valuable v = -au[i];
//        for (auto j=m.size2(); j--; ) {
//            v += m(i,j) * s[j];
//        }
//        BOOST_TEST(v==0);
//    }
////    std::cout << &s << std::endl;
//    auto eq = m.Equation(au);
//    std::cout << eq << std::endl;
//    // c = (ax+by) / z
//
//
//    Valuable v = vm;
//    //std::cout << v << std::endl;
//    // 30*v3*v3 + 12*v2*v3 + 14*v1*v3 + 2*v2*v2 + 2*v1*v2 + 2*v1*v1
//    //    0 0 1 :
//    //    0 1 2 :
//    //    1 0 3 :
//    //    1 1 4 :
//
//    //    0 0 | 1 : 0 != -1
//    //    0 1 | 2 :
//    //    1 0 | 3 :
//    //    1 1 | 4 :
//    
//    // to solve:
//    //    1 1 | 6 :
//    //    1 1 | 4 :
//    // solution : 2 3
//    // :facepalm:
//    //    (2*v2*v2 + 2*v1*v2 + 8*v2 + 2*v1*v1 + 10*v1 - 14) = 0
//    //    (2*9 + 2*6 + 8*3 + 2*4 + 10*2 - 14) = 0
//    //

}

BOOST_AUTO_TEST_CASE(Codec_test, *disabled())
{
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
    auto vm = e.ViewMatrix();
    Valuable vme = vm;
    ublas::vector<Valuable> augment(vm.size1());
    std::cout<< vme <<std::endl;

    Formula f = e;
    for (int i=vm.size1(); i--;) {
        for (int j=vm.size2(); j--;) {
            BOOST_TEST(vm(i,j)==f(i,j));
        }
    }
    
//    std::cout<<f<<std::endl;
//    for (int i=e.size1(); i--;) {
//        for (int j=e.size2(); j--;) {
////            BOOST_TEST(e(i,j)==f(i,j));
//        }
//    }
}
