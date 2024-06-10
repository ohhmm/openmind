//
// Created by Сергей Кривонос on 10.08.17.
//
#define BOOST_TEST_MODULE ImageCodec test
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/gil/extension/io/targa.hpp>
#include "../Extrapolator.h"
#include <boost/log/trivial.hpp>


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
    BOOST_LOG_TRIVIAL(info) << "Starting ImageCodec_test";

    rgba8_image_t src;
    BOOST_LOG_TRIVIAL(info) << "Before reading image";
    read_image(TEST_SRC_DIR "g.tga", src, targa_tag());
    BOOST_LOG_TRIVIAL(info) << "Image read successfully";

    BOOST_LOG_TRIVIAL(info) << "Before writing image";
    write_view(TEST_BIN_DIR "was.tga", view(src), targa_tag());
    BOOST_LOG_TRIVIAL(info) << "Image written successfully";

    auto rows = src.dimensions().y;
    auto cols = src.dimensions().x;
    BOOST_LOG_TRIVIAL(info) << "Before initializing extrapolators";
    Extrapolator a(rows, cols);
    BOOST_LOG_TRIVIAL(info) << "Extrapolator a initialized";
    Extrapolator r(rows, cols);
    BOOST_LOG_TRIVIAL(info) << "Extrapolator r initialized";
    Extrapolator g(rows, cols);
    BOOST_LOG_TRIVIAL(info) << "Extrapolator g initialized";
    Extrapolator b(rows, cols);
    BOOST_LOG_TRIVIAL(info) << "Extrapolator b initialized";

    auto& v = view(src);
    BOOST_LOG_TRIVIAL(info) << "Before populating extrapolators";
    for (auto i = rows; i--;) { // raw
        for (auto j = cols; j--;) { // column
            auto px = v(i,j);
            BOOST_LOG_TRIVIAL(info) << "Populating extrapolators for pixel (" << i << ", " << j << ")";
            a(i,j) = get_color(px,alpha_t());
            r(i,j) = get_color(px,red_t());
            g(i,j) = get_color(px,green_t());
            b(i,j) = get_color(px,blue_t());
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Extrapolators populated";

    Variable x, y, z;
    std::list<Variable> formulaParamSequence = { y, x };
    BOOST_LOG_TRIVIAL(info) << "Before optimizing fa";
    auto fa = a.Factors(y, x, z);
    BOOST_LOG_TRIVIAL(info) << "After creating fa: " << fa;
    fa.optimize();
    BOOST_LOG_TRIVIAL(info) << "fa optimized: " << fa;

    BOOST_LOG_TRIVIAL(info) << "Before optimizing fr";
    auto fr = r.Factors(y, x, z);
    BOOST_LOG_TRIVIAL(info) << "After creating fr: " << fr;
    fr.optimize();
    BOOST_LOG_TRIVIAL(info) << "fr optimized: " << fr;

    BOOST_LOG_TRIVIAL(info) << "Before optimizing fg";
    auto fg = g.Factors(y, x, z);
    BOOST_LOG_TRIVIAL(info) << "After creating fg: " << fg;
    fg.optimize();
    BOOST_LOG_TRIVIAL(info) << "fg optimized: " << fg;

    BOOST_LOG_TRIVIAL(info) << "Before optimizing fb";
    auto fb = b.Factors(y, x, z);
    BOOST_LOG_TRIVIAL(info) << "After creating fb: " << fb;
    fb.optimize();
    BOOST_LOG_TRIVIAL(info) << "fb optimized: " << fb;

    // Unification
    BOOST_LOG_TRIVIAL(info) << "Before unification";
    FormulaOfVaWithSingleIntegerRoot
        afo(z, fa, &formulaParamSequence),
        rfo(z, fr, &formulaParamSequence),
        gfo(z, fg, &formulaParamSequence),
        bfo(z, fb, &formulaParamSequence);
    BOOST_LOG_TRIVIAL(info) << "After unification: " << "afo: " << afo << ", rfo: " << rfo << ", gfo: " << gfo << ", bfo: " << bfo;

    // inbound data deduce
    BOOST_LOG_TRIVIAL(info) << "Before deducing inbound data";
    decltype(src) dst(src.dimensions());
    auto dv = view(dst);
    for (auto i = rows; i--;) { // raw
        for (auto j = cols; j--;) { // column
            auto& d = dv(i, j);
            auto& s = v(i,j);
            BOOST_LOG_TRIVIAL(info) << "Deducing alpha for pixel (" << i << ", " << j << ")";
            get_color(d,alpha_t()) = static_cast<unsigned char>(afo(i,j));
            BOOST_LOG_TRIVIAL(info) << "Deducing red for pixel (" << i << ", " << j << ")";
            get_color(d,red_t()) = static_cast<unsigned char>(rfo(i,j));
            BOOST_LOG_TRIVIAL(info) << "Deducing green for pixel (" << i << ", " << j << ")";
            get_color(d,green_t()) = static_cast<unsigned char>(gfo(i,j));
            BOOST_LOG_TRIVIAL(info) << "Deducing blue for pixel (" << i << ", " << j << ")";
            get_color(d,blue_t()) = static_cast<unsigned char>(bfo(i,j));

            BOOST_LOG_TRIVIAL(info) << "Verifying pixel (" << i << ", " << j << ")";
            BOOST_TEST(unsigned(d[0])==unsigned(s[0]));
            BOOST_TEST(unsigned(d[1])==unsigned(s[1]));
            BOOST_TEST(unsigned(d[2])==unsigned(s[2]));
            BOOST_TEST(unsigned(d[3])==unsigned(s[3]));
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Inbound data deduced successfully.";
    write_view(TEST_BIN_DIR "o.tga", dv, targa_tag());

    // outband data deduce
    BOOST_LOG_TRIVIAL(info) << "Before deducing outband data";
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
            BOOST_LOG_TRIVIAL(info) << "Deducing alpha for pixel (" << r << ", " << c << ")";
            get_color(dv(r, c),alpha_t()) = static_cast<unsigned char>(afo(i,j));
            BOOST_LOG_TRIVIAL(info) << "Deducing red for pixel (" << r << ", " << c << ")";
            get_color(dv(r, c),red_t()) = static_cast<unsigned char>(rfo(i,j));
            BOOST_LOG_TRIVIAL(info) << "Deducing green for pixel (" << r << ", " << c << ")";
            get_color(dv(r, c),green_t()) = static_cast<unsigned char>(gfo(i,j));
            BOOST_LOG_TRIVIAL(info) << "Deducing blue for pixel (" << r << ", " << c << ")";
            get_color(dv(r, c),blue_t()) = static_cast<unsigned char>(bfo(i,j));
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Outband data deduced successfully.";
    write_view(TEST_BIN_DIR "e.tga", dv, targa_tag());

    BOOST_LOG_TRIVIAL(info) << "Completed ImageCodec_test";
}
