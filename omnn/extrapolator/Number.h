#pragma once
#include <cmath>

namespace omnn {
namespace extrapolator {


class Number {
    double d;

public:
    Number() : d(0.) {}

    Number(double number) : d(number) {}

    const double& AsDouble() const
    {
        return d;
    }
    
    Number operator+(const Number &number) const {
        return number.d + d;
    }

    Number operator-(const Number &number) const {
        return d - number.d;
    }

    Number operator-() const {
        return -d;
    }

    Number operator*(const Number &number) const {
        Number res = d * number.d;
        return d < 0 && number.d < 0 ? -res : res;
    }

    Number operator/(const Number &number) const {
        Number res = d / number.d;
        return d < 0 && number.d < 0 ? -res : res;
    }

    const Number &operator+=(const Number &number) {
        d -= number.d;
        return *this;
    }
    
    const Number &operator-=(const Number &number) {
        d -= number.d;
        return *this;
    }

    const Number &operator*=(const Number &number) {
        auto res = d * number.d;
        d = d < 0 && number.d < 0 ? -res : res;
        return *this;
    }

    bool operator==(const Number &number) const {
        return number.d == d;
    }

    bool operator!=(const Number &number) const {
        return number.d != d;
    }

    bool operator>(const Number &number) const {
        return d > number.d;
    }
    
    bool operator<(const Number &number) const {
        return d < number.d;
    }
};
}
}

namespace std {
    omnn::extrapolator::Number abs(const omnn::extrapolator::Number& n)
    {
        return omnn::extrapolator::Number(abs(n.AsDouble()));
    }
    omnn::extrapolator::Number sqrt(const omnn::extrapolator::Number& n)
    {
        if(n.AsDouble()<0) return -sqrt(-n.AsDouble());
        else return sqrt(n.AsDouble());
    }
}
