//
// Created by Sergej Krivonos on 25.02.18.
//
#define BOOST_TEST_MODULE Stasis test
#include <boost/test/unit_test.hpp>

#include "System.h"


using namespace omnn::math;
using namespace boost::unit_test;
using namespace std;

BOOST_AUTO_TEST_CASE(Stasis
                     ,*disabled()
                     ) // Solve magic square
{
    constexpr auto Sz = 5;
    constexpr auto m = (Sz*(Sz*Sz+1))/2;
    constexpr auto mm = -m; 
    DECL_VA(x);
    DECL_VA(y);
    DECL_VA(v);

    auto at = [&](const Valuable& xx, const Valuable& yy, const Valuable& vv) -> Valuable {
        return x.Equals(xx).sq() + y.Equals(yy).sq() + v.Equals(vv).sq();
    };

    System world;
	world << at(0, 0, 1);

    auto getat = [&](const Valuable& xx, const Valuable& yy) {
        /*constexpr*/ static DECL_VA(t);
        return at(xx,yy,t)(t);
    };

    Sum sumd0{mm}, sumd1{mm};
    Valuable::OptimizeOff oo;
    for (auto xx=Sz; xx--;) {
        Sum sumx{mm}, sumy{mm};
        for (auto yy=Sz; yy--; ) {
            sumx.Add(getat(xx,yy));
            sumy.Add(getat(xx,yy));
        }
        world << sumx;
        world << sumy;
        sumd0.Add(getat(xx,xx));
        sumd1.Add(getat(Sz-xx, Sz-xx));
    }
    world << sumd0;
    world << sumd1;

    for (auto xx=Sz; xx--;) {
        auto copy = world;
        copy.Eval(x, xx);
        for (auto yy=Sz; yy--; ) {
            auto co = copy;
            co.Eval(y, yy);
            auto xyValues = co.Solve(v);
            for (auto& s : xyValues) {
            if (!s.IsInt()
//                s.size()!=1
                ) {
                IMPLEMENT
            }else{
                    std::cout << s
//                    << *s.begin()
                    << ' ';
            }
        }
        }
        std::cout << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(Stasis_empty_test){} // to success end if all tests disabled
