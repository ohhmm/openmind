#pragma once
#include <cmath>
#include <iostream>
#include "OpenOps.h"

namespace omnn {
namespace math {


class SymmetricDouble
        : public OpenOps<SymmetricDouble>
{
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

    SymmetricDouble &operator+=(const SymmetricDouble &number) {
        d += number.d;
        return *this;
    }
    
    SymmetricDouble &operator*=(const SymmetricDouble &number) {
        auto res = d * number.d;
        d = d < 0 && number.d < 0 ? -res : res;
        return *this;
    }
    
    SymmetricDouble &operator/=(const SymmetricDouble &number) {
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
    omnn::math::SymmetricDouble abs(const omnn::math::SymmetricDouble& n);
    omnn::math::SymmetricDouble sqrt(const omnn::math::SymmetricDouble& n);
}
