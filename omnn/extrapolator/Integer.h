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
    , public boost::multiprecision::cpp_int
{
    using base = Valuable;
    using base_int = boost::multiprecision::cpp_int;
public:
    using base::base;
    using base_int::base_int;
    Integer(const base_int& i) {
        *this = i;
    }

    Valuable operator -(const Valuable& v) override {return Integer(base_int(0) - v);}
    Valuable& operator +=(const Valuable& v) override {
        base_int::operator+=(dynamic_cast<const base_int&>(dynamic_cast<const Integer&>(v)));
        return *this;
    }
    Valuable& operator *=(const Valuable& v) override {
        base_int::operator*=(dynamic_cast<const base_int&>(dynamic_cast<const Integer&>(v)));
        return *this;
    }
    Valuable& operator /=(const Valuable& v) override {
        base_int::operator/=(dynamic_cast<const base_int&>(dynamic_cast<const Integer&>(v)));
        return *this;
    }
    Valuable& operator %=(const Valuable& v) override {
        base_int::operator%=(dynamic_cast<const base_int&>(dynamic_cast<const Integer&>(v)));
        return *this;
    }
//    Valuable operator--(int) override {
//        return base_int::operator--(int());
//    }
};

}}
