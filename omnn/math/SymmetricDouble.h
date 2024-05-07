#pragma once
#include <omnn/math/OpenOps.h>

#include <cmath>
#include <iostream>


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
    constexpr
    SymmetricDouble() : d(0.) {}

    constexpr
    SymmetricDouble(const_reference_type number) : d(number) {}

    constexpr
    const_reference_type AsDouble() const
    {
        return d;
    }

    constexpr
    SymmetricDouble operator-() const {
        return -d;
    }

    constexpr
    SymmetricDouble &operator+=(const SymmetricDouble &number) {
        d += number.d;
        return *this;
    }

    constexpr
    SymmetricDouble &operator*=(const SymmetricDouble &number) {
        auto res = d * number.d;
        d = d < 0 && number.d < 0 ? -res : res;
        return *this;
    }

    constexpr
    SymmetricDouble &operator/=(const SymmetricDouble &number) {
        auto res = d / number.d;
        d = d < 0 && number.d < 0 ? -res : res;
        return *this;
    }

    bool operator==(const SymmetricDouble &number) const {
        return std::abs(number.d - d) < 0.0000001;
    }

    [[nodiscard]] constexpr
    bool operator<(const SymmetricDouble &number) const {
        return d < number.d;
    }

    friend std::ostream& operator<<(std::ostream& out, const SymmetricDouble& obj)
    {
        out << obj.d;
        return out;
    }

};
}
}

namespace std {
[[nodiscard]] constexpr omnn::math::SymmetricDouble abs(const omnn::math::SymmetricDouble& n) { return omnn::math::SymmetricDouble(abs(n.AsDouble())); }
[[nodiscard]] constexpr omnn::math::SymmetricDouble pow(const omnn::math::SymmetricDouble& base, const omnn::math::SymmetricDouble& exp);
[[nodiscard]] omnn::math::SymmetricDouble sqrt(const omnn::math::SymmetricDouble& n);
} // namespace std
