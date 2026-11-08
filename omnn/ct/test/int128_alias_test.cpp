#define BOOST_TEST_MODULE int128_alias test
#include "omnn/ct/int128.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(int128_alias_usable) {
    omnn::int128_t a = 123;
    omnn::uint128_t b = 456;
    BOOST_REQUIRE(a + static_cast<omnn::int128_t>(b) == 579);
}
