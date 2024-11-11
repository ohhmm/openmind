#include <boost/test/unit_test.hpp>
#include "../Integer.h"
#include "../Variable.h"

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(Integer_bit_operations_test) {
    // Test And operation with positive numbers
    BOOST_TEST((3_v.And(2_v)) == 2);
    BOOST_TEST((5_v.And(3_v)) == 1);
    BOOST_TEST((15_v.And(7_v)) == 7);

    // Test And operation with negative numbers
    BOOST_TEST(((-1_v).And(2_v)) == 2);
    BOOST_TEST(((-5_v).And(3_v)) == 3);
    BOOST_TEST(((-15_v).And(7_v)) == 7);

    // Test Or operation with positive numbers
    BOOST_TEST((3_v.Or(2_v)) == 3);
    BOOST_TEST((5_v.Or(3_v)) == 7);
    BOOST_TEST((15_v.Or(7_v)) == 15);

    // Test Or operation with negative numbers
    BOOST_TEST(((-1_v).Or(2_v)) == -1);
    BOOST_TEST(((-5_v).Or(3_v)) == -5);
    BOOST_TEST(((-15_v).Or(7_v)) == -15);

    // Test Xor operation with positive numbers
    BOOST_TEST((3_v.Xor(2_v)) == 1);
    BOOST_TEST((5_v.Xor(3_v)) == 6);
    BOOST_TEST((15_v.Xor(7_v)) == 8);

    // Test Xor operation with negative numbers
    BOOST_TEST(((-1_v).Xor(2_v)) == -3);
    BOOST_TEST(((-5_v).Xor(3_v)) == -8);
    BOOST_TEST(((-15_v).Xor(7_v)) == -22);

    // Test specific cases from failing tests
    auto v = Variable();
    auto _1 = v + 10;
    auto _2 = v + 11;
    auto _ = _1.Or(5, _2);
    auto t = _;
    t.Eval(v, 0);
    t.optimize();
    BOOST_TEST(t == 11);  // This was failing with 12
    t = _;
    t.Eval(v, 1);
    t.optimize();
    BOOST_TEST(t == 15);  // This was failing with 14
}
