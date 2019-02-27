#define BOOST_TEST_MODULE TS test
#include <boost/test/unit_test.hpp>
#include "Sum.h"
#include "Variable.h"
#include "System.h"


using namespace omnn::math;
using namespace boost::unit_test;

template<typename T>
T bits(T v){
    T bits = 0;
    while (v) {
        ++bits;
        v = v >> 1;
    }
    return bits;
}

BOOST_AUTO_TEST_CASE(TS_1d)
{
//    System s;
//    s.MakeTotalEqu(true);
    // y=f(x), where x is code of sequence
    Variable time, price;
    int points[] = {
        10,1,30,5
    };
    int sz = std::size(points);
    int encodeBits = bits(sz);
    int mask = (1 << encodeBits) - 1;

    auto sys = 0_v;

    // points math data
    Variable x;
    auto ExtractMove = [&](int i){
        auto targetId = x.And((i+1)*encodeBits,mask<<(i*encodeBits)).shr(i*encodeBits);
        return targetId;
    };
    // last mobe back to start
    auto statement = ExtractMove(sz-1).Equals(0);
//    s << statement;
    sys.logic_and(statement);
    // moves uniqueness
    for (int i=1; i<sz; ++i) {
        statement = ExtractMove(i).NotEquals(ExtractMove(i-1));
//        s << statement;
        sys.logic_and(statement);
    }
    // generate function of getting point value by its index
    auto ValueByIdx=[&](Valuable i){
        Variable id;
        static auto MathDataForm = [&](){
            auto data = 1_v;
            Variable val;
            for (int i=0; i<sz; ++i) {
                data.logic_or(id.Equals(i).LogicAnd(val.Equals(points[i])));
            }
            return data(val);
        };
        
        static auto data = MathDataForm();
        
        auto localData = data;
        localData.Eval(id, i);
        return localData;
    };
    // move len square
    auto MoveLenSq=[&](int i)
    {
        assert(i);
        auto prev = ExtractMove(i-1);
        auto target = ExtractMove(i);
        auto diff = ValueByIdx(prev)-ValueByIdx(target);
        return diff ^ 2;
    };
    auto SumSqLens = [&](){
        auto sum = 0_v;
        for (int i=1; i<sz; ++i)
            sum+=MoveLenSq(i);
        return sum;
    };
    auto sumSqLens=SumSqLens();
}

//BOOST_AUTO_TEST_CASE(TS_1d_system,*disabled())
//{
//    System s;
//    s.MakeTotalEqu(true);
//    // y=f(x), where x is code of sequence
//
//    Variable time, price;
//    int points[] = {
//        10,1,30,5
//    };
//    int sz = std::size(points);
//    int encodeBits = bits(sz);
//    int mask = (1 << encodeBits) - 1;
//
//    Variable moves[sz]; // move is next point id
//    Variable idx[sz], val[sz];
//    Variable movelenv[sz];
//    // example, point[0]=10, point[1]=1 : move[0]=1 describes first move made from point[0] to point[1]
//    // move[1]=0 describes next move from point[1] to point[0]
//    // move[i]!=move[i+1]
//    // this is encoded layout in x for y(x)=(sum of shifted moves)
//    Valuable code;
//    for (int i = 0; i < sz; ++i) {
//        code += moves[i].shl(encodeBits*i);
//        s << idx[i].Equals(i) << val[i].Equals(points[i]);
//    }
//    for (int i = 1; i < sz; ++i) {
//        s << moves[1].NotEquals(moves[2]);
//    }
//
//    Variable x;
//    s << x.Equals(code);
//
//    auto max = [](Valuable v1, Valuable v2) {
//        return v1.IfLess(v2, v2, v1);
//    };
//    auto min = [](Valuable v1, Valuable v2) {
//        return v1.IfLess(v2, v1, v2);
//    };
//
//    auto distance = [&](int i1, int i2){
//        return max(val[i1],val[i2])-min(val[i1],val[i2]);
//    };
//
//    auto movelen =[&](int i){
//        ++i;
//        return x.And(encodeBits*i, mask<<(i*encodeBits)).shr(i-1);
//    };
//
//    auto lastIdx = sz-1;
//    s << movelenv[lastIdx].Equals(distance(0,lastIdx));
//    for (int i = lastIdx; i > 0; --i) {
//        s << movelenv[i].Equals(distance(i,i-1));
//    }
//
//    auto sqlen=[&](int i1, int i2){
//        return (moves[i1]-moves[i2])^2;
//    };
//
//    Valuable sqlensum;
//    for (int i = 1; i<sz; ++i) {
//        sqlensum += movelen(i,i-1);
//    }
//
//    Variable lenv;
//    s << lenv.Equals(len);
//
//    auto y = s.Total();
//
//
//    auto extremums = (y).d(x)(x);
//    auto longest = extremums.Top();
//
//
////    auto unix = xpath.equals((xmove[0] << (2 * 2)) + (xmove[1] << 2) + xmove[2]);
////    auto uniy = ypath.equals((xmove[0] << (2 * 2)) + (xmove[1] << 2) + xmove[2]);
////
//    //    auto xext = unix(
//
//
//}

BOOST_AUTO_TEST_CASE(TS_2d)
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
    

}
