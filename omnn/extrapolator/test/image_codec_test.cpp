//
// Created by Сергей Кривонос on 10.08.17.
//
#define BOOST_TEST_MODULE ImageCodec test
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/gil/extension/io/targa.hpp>
#include "Extrapolator.h"

using namespace omnn::math;
using namespace boost::unit_test;
using namespace boost::gil;
using namespace std;


std::string l(const omnn::math::Valuable& v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

BOOST_AUTO_TEST_CASE(ImageCodec_test)
{
    rgba8_image_t src;
    read_image(TEST_SRC_DIR "g.tga", src, targa_tag());
    write_view(TEST_BIN_DIR "was.tga", view(src), targa_tag());
    
    auto rows = src.dimensions().y;
    auto cols = src.dimensions().x;
    Extrapolator a(rows, cols);
    Extrapolator r(rows, cols);
    Extrapolator g(rows, cols);
    Extrapolator b(rows, cols);

    auto v = view(src);
    for (auto i = rows; i--;) { // raw
        for (auto j = cols; j--;) { // column
            auto px = v(i,j);
            a(i,j) = get_color(px,alpha_t());
            r(i,j) = get_color(px,red_t());
            g(i,j) = get_color(px,green_t());
            b(i,j) = get_color(px,blue_t());
        }
    }

    Variable x, y, z;
    std::list<Variable> formulaParamSequence = { y, x };
    auto fa = a.Factors(y, x, z);
//    fa.SetView(Valuable::View::Flat);
    fa.optimize();
    std::cout << fa << std::endl;
    auto fr = r.Factors(y, x, z);
//    fr.SetView(Valuable::View::Flat);
    fr.optimize();
    std::cout << fr << std::endl;
    auto fg = g.Factors(y, x, z);
//    fg.SetView(Valuable::View::Flat);
    fg.optimize();
    std::cout << fg << std::endl;
    auto fb = b.Factors(y, x, z);
//    fb.SetView(Valuable::View::Flat);
    fb.optimize();
    std::cout << fb << std::endl;

    // Unification
    FormulaOfVaWithSingleIntegerRoot
        afo(z, fa, &formulaParamSequence),
        rfo(z, fr, &formulaParamSequence),
        gfo(z, fg, &formulaParamSequence),
        bfo(z, fb, &formulaParamSequence);

    // inbound data deduce
    decltype(src) dst(src.dimensions());
    auto dv = view(dst);
    for (auto i = rows; i--;) { // raw
        for (auto j = cols; j--;) { // column
            auto& d = dv(i, j);
            auto& s = v(i,j);
            get_color(d,alpha_t()) = static_cast<unsigned char>(afo(i,j));
            get_color(d,red_t()) = static_cast<unsigned char>(rfo(i,j));
            get_color(d,green_t()) = static_cast<unsigned char>(gfo(i,j));
            get_color(d,blue_t()) = static_cast<unsigned char>(bfo(i,j));

            BOOST_TEST(unsigned(d[0])==unsigned(s[0]));
            BOOST_TEST(unsigned(d[1])==unsigned(s[1]));
            BOOST_TEST(unsigned(d[2])==unsigned(s[2]));
            BOOST_TEST(unsigned(d[3])==unsigned(s[3]));
        }
    }
    write_view(TEST_BIN_DIR "o.tga", dv, targa_tag());
    
    // outband data deduce
    afo.SetMode(FormulaOfVaWithSingleIntegerRoot::Newton);
    afo.SetMin(0); afo.SetMax(255);
    rfo.SetMode(FormulaOfVaWithSingleIntegerRoot::Newton);
    rfo.SetMin(0); rfo.SetMax(255);
    gfo.SetMode(FormulaOfVaWithSingleIntegerRoot::Newton);
    gfo.SetMin(0); gfo.SetMax(255);
    bfo.SetMode(FormulaOfVaWithSingleIntegerRoot::Newton);
    bfo.SetMin(0); bfo.SetMax(255);
    
    const auto d = 2; // 5
    cols+=d;rows+=d;
    dst = decltype(src)(rows+d, cols+d);
    dv = view(dst);
    for (auto i = rows; --i>=-d;) { // raw
        for (auto j = cols; --j>=-d;) { // column
            auto c=j+d;
            auto r = i+d;
            auto& d = dv(r, c);
            get_color(d,alpha_t()) = static_cast<unsigned char>(afo(i,j));
            get_color(d,red_t()) = static_cast<unsigned char>(rfo(i,j));
            get_color(d,green_t()) = static_cast<unsigned char>(gfo(i,j));
            get_color(d,blue_t()) = static_cast<unsigned char>(bfo(i,j));
        }
    }
    write_view(TEST_BIN_DIR "e.tga", dv, targa_tag());
}

