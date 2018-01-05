//
// Created by Сергей Кривонос on 10.08.17.
//
#define BOOST_TEST_MODULE ImageCodec test
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/gil/extension/io/bmp_all.hpp>
#include <boost/gil/extension/io/targa_all.hpp>
#include "Extrapolator.h"

using namespace omnn::extrapolator;
using namespace boost::unit_test;
using namespace boost::gil;
using namespace std;


std::string l(const omnn::extrapolator::Valuable& v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

BOOST_AUTO_TEST_CASE(ImageCodec_test)
{
    rgba8_image_t src;
    read_image(TEST_SRC_DIR "gresized.tga", src, targa_tag());
    
    auto rows = src.dimensions().y;
    auto cols = src.dimensions().x;
    Extrapolator ex(rows, cols);
    Extrapolator a(rows, cols);
    Extrapolator r(rows, cols);
    Extrapolator g(rows, cols);
    Extrapolator b(rows, cols);

    auto v = view(src);
    for (auto i=ex.size1(); i--;) { // raw
        for (auto j = ex.size2(); j--;) {// column
            auto px = v(i,j);
            a(i,j) = px[0];
            r(i,j) = px[1];
            g(i,j) = px[2];
            b(i,j) = px[3];
            
        }
    }
    
    Variable x, y, z;
    auto f = ex.Factors(y, x, z);
    std::list<Variable> formulaParamSequence = { y, x };
    FormulaOfVaWithSingleIntegerRoot fo(z, f, &formulaParamSequence);

    // TODO : extrapolation
//    std::cout << fo(ex.size1(), ex.size2()) << std::endl;

    // inbound data deduce
    rgb8_image_t dst(src.dimensions());
    auto dv = view(dst);
    for (auto i=ex.size1(); i--;) { // raw
        for (auto j = ex.size2(); j--;) { // column
            auto c = fo(i, j);
            std::cout << c.str() << std::endl;
            BOOST_TEST(c == ex(i, j));
            dv(i, j) = rgb8_pixel_t(int(c));
            auto d=dv(i,j);
            auto s=v(i,j);
//            BOOST_TEST(unsigned(d[0])==unsigned(s[0]));
//            BOOST_TEST(unsigned(d[1])==unsigned(s[1]));
//            BOOST_TEST(unsigned(d[2])==unsigned(s[2]));
//            BOOST_TEST(unsigned(d[3])==unsigned(s[3]));
        }
    }
    
    
    write_view(TEST_BIN_DIR "o.tga", v, targa_tag());
}

