//
// Created by Сергей Кривонос on 10.08.17.
//
#define BOOST_TEST_MODULE ImageCodec test
#include <boost/test/unit_test.hpp>
#include "Extrapolator.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/atomic/atomic_ref.hpp>
#include <boost/gil/extension/io/targa.hpp>

#include <omnn/rt/tasq.h>


using namespace omnn::math;

using namespace boost::unit_test;
using namespace boost::gil;

namespace std {

    auto& operator<<(auto& stream, alpha_t) {
        stream << "alpha";
        return stream;
    }
    auto& operator<<(auto& stream, red_t) {
        stream << "red";
        return stream;
    }
    auto& operator<<(auto& stream, green_t) {
        stream << "green";
        return stream;
    }
    auto& operator<<(auto& stream, blue_t) {
        stream << "blue";
        return stream;
    }

}

using namespace std;


std::string l(const omnn::math::Valuable& v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

DECL_VARS(x, y, z);

#if !defined(NDEBUG) || defined(MSVC)

BOOST_AUTO_TEST_CASE(ImageCodec_test)
{
    rgba8_image_t src;
    read_image(TEST_SRC_DIR "g.tga", src, targa_tag());
    auto& v = view(src);
    write_view(TEST_BIN_DIR "was.tga", v, targa_tag());
    auto rows = src.dimensions().y;
    auto cols = src.dimensions().x;
    std::list<Variable> formulaParamSequence = { y, x };

    auto get_color_formula = [&](auto tag) {
        auto extrapolator = Extrapolator(rows, cols);
        for (auto i = rows; i--;) { // raw
            for (auto j = cols; j--;) { // column
                auto px = v(i,j);
                extrapolator(i,j) = get_color(px, tag);
            }
        }
        auto factors = extrapolator.Factors(y, x, z);
        // BOOST_CHECK_NO_THROW(factors.optimize());
        std::cout << "\n Input image formula for " << tag << " chennel: " << factors << std::endl;

        return FormulaOfVaWithSingleIntegerRoot(z, factors, &formulaParamSequence);
    };

    omnn::rt::StoringTasksQueue<FormulaOfVaWithSingleIntegerRoot> tasksInParallel(false);
    tasksInParallel.AddTask(get_color_formula, alpha_t());
    tasksInParallel.AddTask(get_color_formula, red_t());
    tasksInParallel.AddTask(get_color_formula, green_t());
    tasksInParallel.AddTask(get_color_formula, blue_t());

    // Unification
    auto afo = tasksInParallel.PeekNextResult();
    auto rfo = tasksInParallel.PeekNextResult();
    auto gfo = tasksInParallel.PeekNextResult();
    auto bfo = tasksInParallel.PeekNextResult();

    // inbound data deduce
    decltype(src) dst(src.dimensions());
    auto dv = view(dst);
    for (auto i = rows; i--;) {     // raw
        for (auto j = cols; j--;) { // column
            auto& d = dv(i, j);
            auto& s = v(i, j);
            get_color(d, alpha_t()) = static_cast<unsigned char>(afo(i, j));
            get_color(d, red_t()) = static_cast<unsigned char>(rfo(i, j));
            get_color(d, green_t()) = static_cast<unsigned char>(gfo(i, j));
            get_color(d, blue_t()) = static_cast<unsigned char>(bfo(i, j));

            BOOST_TEST(unsigned(d[0]) == unsigned(s[0]));
            BOOST_TEST(unsigned(d[1]) == unsigned(s[1]));
            BOOST_TEST(unsigned(d[2]) == unsigned(s[2]));
            BOOST_TEST(unsigned(d[3]) == unsigned(s[3]));
        }
    }
}

#else
BOOST_AUTO_TEST_CASE(ImageCodec_extrapolated_test)
{
    rgba8_image_t src;
    read_image(TEST_SRC_DIR "g.tga", src, targa_tag());
    auto& v = view(src);
    write_view(TEST_BIN_DIR "was.tga", v, targa_tag());
    auto rows = src.dimensions().y;
    auto cols = src.dimensions().x;
    std::list<Variable> formulaParamSequence = { y, x };

    auto get_color_formula = [&](auto tag) {
        auto extrapolator = Extrapolator(rows, cols);
        for (auto i = rows; i--;) { // raw
            for (auto j = cols; j--;) { // column
                auto px = v(i,j);
                extrapolator(i,j) = get_color(px, tag);
            }
        }
        auto factors = extrapolator.Factors(y, x, z);
        //BOOST_CHECK_NO_THROW(factors.optimize()); //FIXME: uncomment
        std::cout << "\n Input image formula for " << tag << " chennel: " << factors << std::endl;

        return FormulaOfVaWithSingleIntegerRoot(z, factors, &formulaParamSequence);
    };

    omnn::rt::StoringTasksQueue<FormulaOfVaWithSingleIntegerRoot> tasksInParallel(false);
    tasksInParallel.AddTask(get_color_formula, alpha_t());
    tasksInParallel.AddTask(get_color_formula, red_t());
    tasksInParallel.AddTask(get_color_formula, green_t());
    tasksInParallel.AddTask(get_color_formula, blue_t());

    // Unification
    auto afo = tasksInParallel.PeekNextResult();
    auto rfo = tasksInParallel.PeekNextResult();
    auto gfo = tasksInParallel.PeekNextResult();
    auto bfo = tasksInParallel.PeekNextResult();

    // inbound data deduce
    decltype(src) dst(src.dimensions());
    auto dv = view(dst);
    {
        omnn::rt::StoringTasksQueue<void> tasksInParallel(false);
        for (auto i = rows; i--;) {     // raw
            for (auto j = cols; j--;) { // column
                auto& d = dv(i, j);
                tasksInParallel.AddTask([&] {
                    boost::atomic_ref(get_color(dv(i, j), alpha_t())) = static_cast<unsigned char>(afo(i, j));
                });
                tasksInParallel.AddTask([&] {
                    boost::atomic_ref(get_color(dv(i, j), red_t())) = static_cast<unsigned char>(rfo(i, j));
                });
                tasksInParallel.AddTask([&] {
                    boost::atomic_ref(get_color(dv(i, j), green_t())) = static_cast<unsigned char>(gfo(i, j));
                });
                tasksInParallel.AddTask([&] {
                    boost::atomic_ref(get_color(dv(i, j), blue_t())) = static_cast<unsigned char>(bfo(i, j));
                });
            }
        }
    }
    write_view(TEST_BIN_DIR "o.tga", dv, targa_tag());
    for (auto i = rows; i--;) { // raw
        for (auto j = cols; j--;) { // column
            auto& d = dv(i, j);
            auto& s = v(i,j);
            BOOST_TEST(unsigned(boost::atomic_ref(d[0]))==unsigned(s[0]));
            BOOST_TEST(unsigned(boost::atomic_ref(d[1]))==unsigned(s[1]));
            BOOST_TEST(unsigned(boost::atomic_ref(d[2]))==unsigned(s[2]));
            BOOST_TEST(unsigned(boost::atomic_ref(d[3]))==unsigned(s[3]));
        }
    }
    // extrapolating
    afo.SetMode(FormulaOfVaWithSingleIntegerRoot::Newton);
    rfo.SetMode(FormulaOfVaWithSingleIntegerRoot::Newton);
    gfo.SetMode(FormulaOfVaWithSingleIntegerRoot::Newton);
    bfo.SetMode(FormulaOfVaWithSingleIntegerRoot::Newton);

    const auto d = 1;
    cols+=d;rows+=d;
    dst = decltype(src)(rows+d, cols+d);
    dv = view(dst);
    {
        omnn::rt::StoringTasksQueue<void> tasksInParallel(false);
        for (auto i = rows; --i >= -d;) {     // raw
            for (auto j = cols; --j >= -d;) { // column
                auto c = j + d;
                auto r = i + d;
                auto& d = dv(r, c);
                tasksInParallel.AddTask([&] {
                    boost::atomic_ref(get_color(dv(i, j), alpha_t())) = static_cast<unsigned char>(afo(i, j));
                });
                tasksInParallel.AddTask([&] {
                    boost::atomic_ref(get_color(dv(i, j), red_t())) = static_cast<unsigned char>(rfo(i, j));
                });
                tasksInParallel.AddTask([&] {
                    boost::atomic_ref(get_color(dv(i, j), green_t())) = static_cast<unsigned char>(gfo(i, j));
                });
                tasksInParallel.AddTask([&] {
                    boost::atomic_ref(get_color(dv(i, j), blue_t())) = static_cast<unsigned char>(bfo(i, j));
                });
            }
        }
    }
    write_view(TEST_BIN_DIR "e.tga", dv, targa_tag());
}

#endif
