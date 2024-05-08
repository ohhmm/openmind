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
    std::cout << "Starting TS_1d test case" << std::endl;
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

    std::cout << "Initialized variables" << std::endl;

    auto sys = constants::zero;

    // points math data
    DECL_VA(x);
    auto ExtractMove = [&](auto&& i) {
        std::cout << "Extracting move for i = " << i << std::endl;
        auto targetId = x.And((i + 1) * encodeBits, mask << (i * encodeBits)).shr(i * encodeBits);
        std::cout << "Extracted move for i = " << i << ": " << targetId << std::endl;
        return targetId;
    };
    // last move back to start
    auto statement = ExtractMove(lastItemIdx).Equals(constants::zero);
    std::cout << "Checking last move back to start" << std::endl;
    //    s << statement;
    sys += statement.sq();
    std::cout << "Last move back to start checked" << std::endl;
    // moves uniqueness
    for (auto i = 1; i < n; ++i) {
        std::cout << "Processing move uniqueness for i = " << i << std::endl;
        statement = ExtractMove(i).NotEquals(
            ExtractMove(i - 1)); // FIXME: NotEquals behaviour changed. It was 0 for a!=b and 1 for a==b.  then it
                                 // became  /=(a-b) like if a=b then its a division by zero which must exclude branches,
                                 // but does it or it just breaks expression
                                 //        s << statement;
        sys += statement.sq();
        std::cout << "Processed move uniqueness for i = " << i << std::endl;
    }
    std::cout << "Completed move uniqueness checks" << std::endl;
    // generate function of getting point value by its index
    auto ValueByIdx = [&](Valuable i) {
        std::cout << "Getting value by index: " << i << std::endl;
        DECL_VA(id);
        static auto MathDataForm = [&]() {
            auto data = constants::one;
            DECL_VA(val);
            for (int i = 0; i < n; ++i) {
                data.logic_or(id.Equals(i).logic_and(val.Equals(points[i])));
            }
            return data(val);
        };

        static auto data = MathDataForm();

        auto localData = data;
        localData.Eval(id, i);
        std::cout << "Got value by index: " << i << " = " << localData << std::endl;
        return localData;
    };
    // move len square
    auto MoveLenSq = [&](int i) {
        std::cout << "Calculating move length square for i = " << i << std::endl;
        assert(i);
        auto prev = ExtractMove(i - 1);
        auto target = ExtractMove(i);
        auto diff = ValueByIdx(prev) - ValueByIdx(target);
        auto lenSq = diff ^ 2;
        std::cout << "Calculated move length square for i = " << i << ": " << lenSq << std::endl;
        return lenSq;
    };
    auto SumSqLens = [&]() {
        std::cout << "Summing square lengths" << std::endl;
        auto sum = constants::zero;
        for (auto i = 1; i < n; ++i) {
            sum += MoveLenSq(i);
            std::cout << "Sum after i = " << i << ": " << sum << std::endl;
        }
        std::cout << "Completed summing square lengths: " << sum << std::endl;
        return sum;
    };
    auto sumSqLens = SumSqLens();
    std::cout << "Completed TS_1d test case" << std::endl;
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
