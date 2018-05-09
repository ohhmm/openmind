//
// Created by Sergej Krivonos on 4/19/18.
//
#define BOOST_TEST_MODULE Sh test
#include <boost/test/unit_test.hpp>

#include "Variable.h"

using namespace omnn::math;
using namespace boost::unit_test;
using namespace std;

BOOST_AUTO_TEST_CASE(And_test)
{
    Variable v;
    auto _ = v.And(2,3);
    _.Eval(v, 1);
    _.optimize();
    BOOST_TEST(_==1);
    
    _ = v.And(2,3);
    _.Eval(v, 2);
    _.optimize();
    BOOST_TEST(_==2);
    
    _ = v.And(2,3);
    _.Eval(v, 3);
    _.optimize();
    BOOST_TEST(_==3);
    
    auto _1 = v+10;
    auto _2 = v+11;
    _ = _1.And(5, _2);
    auto t = _;
    t.Eval(v,0);
    t.optimize();
    BOOST_TEST(t == 10);
    t = _;
    t.Eval(v,1);
    t.optimize();
    BOOST_TEST(t == 8);
}

BOOST_AUTO_TEST_CASE(Or_test)
{
    Variable v;
    auto _1 = v+10;
    auto _2 = v+11;
    auto _ = _1.Or(5, _2);
    auto t = _;
    t.Eval(v,0);
    t.optimize();
    BOOST_TEST(t == 11);
    t = _;
    t.Eval(v,1);
    t.optimize();
    BOOST_TEST(t == 15);
}

BOOST_AUTO_TEST_CASE(XOr_test)
{
    Variable v;
    auto _1 = v+10;
    auto _2 = v+11;
    auto _ = _1.Xor(5, _2);
    auto t = _;
    t.Eval(v,0);
    t.optimize();
    BOOST_TEST(t == 1);
    t = _;
    t.Eval(v,1);
    t.optimize();
    BOOST_TEST(t == 7);
}

BOOST_AUTO_TEST_CASE(Cyclic_test)
{
    Variable v;
    auto _1 = v+10;
    auto _ = _1.Cyclic(5, 3);
    auto t = _;
    t.Eval(v,0);
    t.optimize();
    BOOST_TEST(t == 18);
    
    _ = _1.Cyclic(5, -1);
    t = _;
    t.Eval(v,0);
    t.optimize();
    BOOST_TEST(t == 5);
    
    _ = _1.Cyclic(5, -2);
    t = _;
    t.Eval(v,0);
    t.optimize();
    BOOST_TEST(t == 18);
}

BOOST_AUTO_TEST_CASE(Sh_test)
{
    // http://static.righto.com/images/bitcoin/block_diagram_ghash.png
    auto _ = "0x02000000"_v
            "17975b97c18ed1f7e255adf297599b55"_v
            "330edab87803c8170100000000000000"_v
            "8a97295a2747b4f1a0b3948df3990344"_v
            "c0e19fa6b2b92b3a19c8e6badc141787"_v
            "358b0553"_v
            "535f0119"_v
            "48750833"_v
            "63"_v; // 161b
    auto sh = "0x0000000000000000"_v
                "e067a478024addfe"_v
                "cdc93628978aa52d"_v
                "91fabd4292982a50"_v;
//    BOOST_TEST(_.sh(161)==sh);
}
