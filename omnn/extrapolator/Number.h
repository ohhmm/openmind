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
    double d;

public:
    SymmetricDouble() : d(0.) {}

    SymmetricDouble(double number) : d(number) {}

    const double& AsDouble() const
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
    
    friend SymmetricDouble operator "" _sd(long double d)
    {
        double val = d;
        return SymmetricDouble(val);
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
