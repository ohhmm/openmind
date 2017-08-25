#pragma once
#include <cmath>
#include <iostream>
#include <boost/operators.hpp>

namespace omnn {
namespace extrapolator {

class SymmetricDouble;


class ops
        : public boost::operators<SymmetricDouble>
{
    template<class T>
    friend bool operator<=(const T& x, const SymmetricDouble& y) { return !static_cast<bool>(x > y); }
    template<class T> friend bool operator>=(const T& x, const SymmetricDouble& y) { return !static_cast<bool>(x < y); }
    template<class T> friend bool operator>(const SymmetricDouble& x, const T& y)  { return y < x; }
    template<class T> friend bool operator<(const SymmetricDouble& x, const T& y)  { return y > x; }
    template<class T> friend bool operator<=(const SymmetricDouble& x, const T& y) { return !static_cast<bool>(y < x); }
    template<class T> friend bool operator>=(const SymmetricDouble& x, const T& y) { return !static_cast<bool>(y > x); }

};

class SymmetricDouble : ops {
    using value_type = long double;
    using reference_type = value_type&;
    using const_reference_type = const value_type&;
    value_type d;

public:
    SymmetricDouble() : d(0.) {}

    SymmetricDouble(const_reference_type number) : d(number) {}

    const_reference_type AsDouble() const
    {
        return d;
    }
    
    SymmetricDouble operator-() const {
        return -d;
    }

    const SymmetricDouble &operator+=(const SymmetricDouble &number) {
        d += number.d;
        return *this;
    }
    
    const SymmetricDouble &operator-=(const SymmetricDouble &number) {
        d -= number.d;
        return *this;
    }

    const SymmetricDouble &operator*=(const SymmetricDouble &number) {
        auto res = d * number.d;
        d = d < 0 && number.d < 0 ? -res : res;
        return *this;
    }
    
    const SymmetricDouble &operator/=(const SymmetricDouble &number) {
        auto res = d / number.d;
        d = d < 0 && number.d < 0 ? -res : res;
        return *this;
    }

    bool operator==(const SymmetricDouble &number) const {
        return std::abs(number.d - d) < 0.0000001;
    }

    bool operator<(const SymmetricDouble &number) const {
        return d < number.d;
    }

    friend std::ostream& operator<<(std::ostream& out, const SymmetricDouble& obj)
    {
        out << obj.d;
        return out;
    }
    
    friend SymmetricDouble operator "" _sd(value_type d)
    {
        return SymmetricDouble(d);
    }
    
};
}
}

namespace std {
    omnn::extrapolator::SymmetricDouble abs(const omnn::extrapolator::SymmetricDouble& n)
    {
        return omnn::extrapolator::SymmetricDouble(abs(n.AsDouble()));
    }
    omnn::extrapolator::SymmetricDouble sqrt(const omnn::extrapolator::SymmetricDouble& n)
    {
        if(n.AsDouble()<0) return -sqrt(-n.AsDouble());
        else return sqrt(n.AsDouble());
    }
}
