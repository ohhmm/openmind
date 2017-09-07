//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include "Valuable.h"

namespace omnn{
namespace extrapolator {

class Integer
    : public Valuable
{
    using base = Valuable;
    using base_int = boost::multiprecision::cpp_int;
    base_int arbitrary = 0;
public:
    using base::base;
//    using base_int::base_int;
    template<class T>
    Integer(const T& i) {
        arbitrary = i;
    }

    Valuable operator -(const Valuable& v) const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override;
    Valuable& operator ++() override;
    
    bool operator <(const Valuable& v) const override;
            
//    bool operator>(int v) const  {
//        return arbitrary > v;
//    }
            
    bool operator ==(const Valuable& v) const override;
            
    friend std::ostream& operator <<(std::ostream& out, const Integer& obj);
};


}
}
