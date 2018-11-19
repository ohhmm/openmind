//
// Created by Sergej Krivonos on 25.02.18.
//
#define BOOST_TEST_MODULE Stasis test
#include <boost/test/unit_test.hpp>

#include "Variable.h"


using namespace omnn::math;
using namespace boost::unit_test;
using namespace std;

BOOST_AUTO_TEST_CASE(Stasis) // Solve magic square
{
    constexpr uint Sz = 5;
    constexpr uint m = (Sz*(Sz*Sz+1))/2;
    auto x = "x"_va,
         y = "y"_va,
         v = "v"_va;

    auto at = [&](const Valuable& xx, const Valuable& yy, const Valuable& vv) -> Valuable {
        return x.Equals(xx).LogicAnd(y.Equals(yy)).LogicAnd(v.Equals(vv));
    };

    auto world = at(0,0,1);

    auto getat = [&](const Valuable& xx, const Valuable& yy) {
        /*constexpr*/static Variable t = "t"_va;
        return at(xx,yy,t)(t);
    };

    auto sumd0 = 0_v, sumd1 = 0_v;
    Valuable::optimizations = {};
    for (auto xx=Sz; xx--;) {
        auto sumx=0_v, sumy=0_v;
        for (auto yy=Sz; yy--; ) {
            sumx += getat(xx,yy);
            sumy += getat(xx,yy);
        }
        world += sumx.Equals(m).sq();
        world += sumy.Equals(m).sq();
        sumd0 += getat(xx,xx);
        sumd1 += getat(Sz-xx, Sz-xx);
    }
    world += sumd0.Equals(m).sq();
    world += sumd1.Equals(m).sq();

    for (auto xx=Sz; xx--;) {
        for (auto yy=Sz; yy--; ) {

            auto co = world.eval({{x,xx},{y,yy}});
            Valuable::optimizations = true;
            co.optimize();
            Valuable::optimizations = {};
            auto s = co(v);
            if (!s.IsInt()
//                s.size()!=1
                ) {
                IMPLEMENT
            }else{
                std::cout
                    << s
//                    << *s.begin()
                    << ' ';
            }
        }
        std::cout << std::endl;
    }
}

