//
// Created by Сергей Кривонос on 10.08.17.
//
#define BOOST_TEST_MODULE Extrapolator test
#include <boost/test/unit_test.hpp>
#include "Extrapolator.h"

using namespace omnn::extrapolator;
using namespace boost::unit_test;

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

BOOST_AUTO_TEST_CASE(Codec_test)
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

    Expression exp = e.operator omnn::extrapolator::Expression();
}
