#define BOOST_TEST_MODULE TS test
#include <boost/test/unit_test.hpp>
#include <array>
#include "Sum.h"
#include "Variable.h"
#include "System.h"
#include <iostream>

using namespace omnn::math;
using namespace boost::unit_test;

template<typename T>
T bits_in_use(T v){
    T bits = 0;
    while (v) {
        ++bits;
        v = v >> 1;
    }
    return bits;
}

/// Control theory generalization hints are here: finds best moves/goods for single dimension
BOOST_AUTO_TEST_CASE(TS_1d) {
    std::cout << "TS_1d test case - Start" << std::endl;
    //    System s;
    //    s.MakeTotalEqu(true);
    // y=f(x), where x is code of sequence
    DECL_VARS(time, price);
    auto points = std::vector({10, 1, 30, 5});
    auto n = std::size(points);
    auto lastItemIdx = n;
    --lastItemIdx;
    auto encodeBits = bits_in_use(n);
    auto mask = (1 << encodeBits) - 1;

    std::cout << "TS_1d test case - Initialized variables" << std::endl;

    auto sys = constants::zero;

    // points math data
    std::cout << "TS_1d test case - Starting points math data" << std::endl;
    DECL_VA(x);
    auto ExtractMove = [&](auto&& i) {
        std::cout << "Extracting move for i = " << i << std::endl;
        std::cout << "TS_1d test case - Before ExtractMove" << std::endl;
        auto targetId = x.And((i + 1) * encodeBits, mask << (i * encodeBits)).shr(i * encodeBits);
        std::cout << "TS_1d test case - After ExtractMove" << std::endl;
        std::cout << "Extracted move for i = " << i << ": " << targetId << std::endl;
        return targetId;
    };
    // last move back to start
    auto statement = ExtractMove(lastItemIdx).Equals(constants::zero);
    std::cout << "Checking last move back to start" << std::endl;
    //    s << statement;
    sys += statement.sq();
    std::cout << "Last move back to start checked" << std::endl;
    std::cout << "TS_1d test case - Completed points math data" << std::endl;

    // moves uniqueness
    std::cout << "TS_1d test case - Starting moves uniqueness checks" << std::endl;
    for (auto i = 1; i < n; ++i) {
        std::cout << "Processing move uniqueness for i = " << i << " - Start" << std::endl;
        statement = ExtractMove(i).NotEquals(
            ExtractMove(i - 1)); // FIXME: NotEquals behaviour changed. It was 0 for a!=b and 1 for a==b.  then it
                                 // became  /=(a-b) like if a=b then its a division by zero which must exclude branches,
                                 // but does it or it just breaks expression
                                 //        s << statement;
        sys += statement.sq();
        std::cout << "Processed move uniqueness for i = " << i << " - End" << std::endl;
    }
    std::cout << "TS_1d test case - Completed moves uniqueness checks" << std::endl;

    // generate function of getting point value by its index
    std::cout << "TS_1d test case - Starting generate function of getting point value by its index" << std::endl;
    auto ValueByIdx = [&](Valuable i) {
        std::cout << "Getting value by index: " << i << std::endl;
        DECL_VA(id);
        static auto MathDataForm = [&]() {
            std::cout << "TS_1d test case - Before MathDataForm" << std::endl;
            auto data = constants::one;
            DECL_VA(val);
            for (int i = 0; i < n; ++i) {
                data.logic_or(id.Equals(i).logic_and(val.Equals(points[i])));
            }
            std::cout << "TS_1d test case - After MathDataForm" << std::endl;
            return data(val);
        };

        static auto data = MathDataForm();

        auto localData = data;
        std::cout << "TS_1d test case - Before Eval" << std::endl;
        localData.Eval(id, i);
        std::cout << "TS_1d test case - After Eval" << std::endl;
        std::cout << "Got value by index: " << i << " = " << localData << std::endl;
        return localData;
    };
    std::cout << "TS_1d test case - Completed generate function of getting point value by its index" << std::endl;

    // move len square
    std::cout << "TS_1d test case - Starting move len square calculations" << std::endl;
    auto MoveLenSq = [&](int i) {
        std::cout << "Calculating move length square for i = " << i << std::endl;
        assert(i);
        auto prev = ExtractMove(i - 1);
        auto target = ExtractMove(i);
        auto diff = ValueByIdx(prev) - ValueByIdx(target);
        std::cout << "TS_1d test case - Before MoveLenSq" << std::endl;
        auto lenSq = diff ^ 2;
        std::cout << "TS_1d test case - After MoveLenSq" << std::endl;
        std::cout << "Calculated move length square for i = " << i << ": " << lenSq << std::endl;
        return lenSq;
    };
    std::cout << "TS_1d test case - Completed move len square calculations" << std::endl;

    // sum square lengths
    std::cout << "TS_1d test case - Starting sum square lengths" << std::endl;
    auto SumSqLens = [&]() {
        std::cout << "TS_1d test case - Before SumSqLens" << std::endl;
        auto sum = constants::zero;
        for (auto i = 1; i < n; ++i) {
            sum += MoveLenSq(i);
            std::cout << "Sum after i = " << i << ": " << sum << std::endl;
        }
        std::cout << "TS_1d test case - After SumSqLens" << std::endl;
        std::cout << "Completed summing square lengths: " << sum << std::endl;
        return sum;
    };
    auto sumSqLens = SumSqLens();
    std::cout << "TS_1d test case - Completed sum square lengths" << std::endl;

    std::cout << "TS_1d test case - End" << std::endl;
}

BOOST_AUTO_TEST_CASE(TS_2d
                     ,*disabled()
                     )
{
    Variable time, price;
    std::pair<Valuable,Valuable> points[] = {
        {0,0},
        {1,5},
        {3,8},
    };
    auto bits = 2;

    Variable xpath, ypath;
    Variable xmove[std::size(points)], ymove[std::size(points)];
//
//    auto unix = xpath.equals((xmove[0] << (2 * 2)) + (xmove[1] << 2) + xmove[2]);
//    auto uniy = ypath.equals((xmove[0] << (2 * 2)) + (xmove[1] << 2) + xmove[2]);

//    auto xext = unix(

    IMPLEMENT
}
