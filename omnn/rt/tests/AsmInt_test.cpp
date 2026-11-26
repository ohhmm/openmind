#define BOOST_TEST_MODULE AsmInt
#include <boost/test/unit_test.hpp>

#include "AsmInt.h"

#include <limits>

namespace {

using omnn::rt::AsmInt;
using omnn::rt::Word;
using boost::multiprecision::cpp_int;

} // namespace

BOOST_AUTO_TEST_CASE(PrefixIncrement)
{
    constexpr auto max = std::numeric_limits<Word>::max();
    struct Case {
        AsmInt input;
        std::size_t resultSize;
    };
    AsmInt longCarry(1024, max);
    longCarry.back() = 7;
    const Case cases[] = {
        {{}, 1},
        {{0}, 1},
        {{7, 42}, 2},
        {{max}, 2},
        {{max, max, 7, max}, 4},
        {{max, 0, 0}, 3},
        {longCarry, 1024},
        {AsmInt(1024, max), 1025},
    };

    for (const auto& test : cases) {
        auto value = test.input;
        const cpp_int expected = static_cast<cpp_int>(value) + 1;
        BOOST_TEST_CONTEXT("input size " << value.size() << ", result size " << test.resultSize) {
            BOOST_CHECK(&++value == &value);
            BOOST_TEST(value.size() == test.resultSize);
            BOOST_TEST(static_cast<cpp_int>(value) == expected);
        }
    }
}
