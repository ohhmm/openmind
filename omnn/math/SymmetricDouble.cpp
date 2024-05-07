#include "SymmetricDouble.h"

#include <cmath>
#include <string>


namespace std {

    [[nodiscard]]
    constexpr
    omnn::math::SymmetricDouble pow(const omnn::math::SymmetricDouble& base, const omnn::math::SymmetricDouble& exp) {
        return omnn::math::SymmetricDouble(std::pow(base.AsDouble(), exp.AsDouble()));
    }

    omnn::math::SymmetricDouble sqrt(const omnn::math::SymmetricDouble& n)
    {
        return omnn::math::SymmetricDouble{
            n.AsDouble() < 0
            ? -sqrt(-n.AsDouble())
            : sqrt(n.AsDouble())
        };
    }

} // namespace std

