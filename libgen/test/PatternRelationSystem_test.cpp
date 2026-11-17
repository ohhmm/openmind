#define BOOST_TEST_MODULE Pattern relation image test
#include <boost/test/unit_test.hpp>

#include "system.hpp"

#include <sstream>

BOOST_AUTO_TEST_CASE(Gradient_pixels)
{
    std::istringstream input("width=8\nheight=4\nred=(x*4)%256\ngreen=(y*4)%256\nblue=64\nalpha=255\n");
    gen::PatternRelationSystem system(input);
    auto image = system.GeneratePatternImage();
    auto pixels = boost::gil::const_view(image);
    BOOST_REQUIRE_EQUAL(pixels.width(), 8);
    BOOST_REQUIRE_EQUAL(pixels.height(), 4);
    for (int y = 0; y < pixels.height(); ++y) {
        for (int x = 0; x < pixels.width(); ++x) {
            const auto& pixel = pixels(x, y);
            BOOST_TEST_CONTEXT("pixel " << x << ',' << y) {
                BOOST_TEST(static_cast<unsigned>(boost::gil::get_color(pixel, boost::gil::red_t())) == x * 4);
                BOOST_TEST(static_cast<unsigned>(boost::gil::get_color(pixel, boost::gil::green_t())) == y * 4);
                BOOST_TEST(static_cast<unsigned>(boost::gil::get_color(pixel, boost::gil::blue_t())) == 64);
                BOOST_TEST(static_cast<unsigned>(boost::gil::get_color(pixel, boost::gil::alpha_t())) == 255);
            }
        }
    }
}
