#include "SymmetricDouble.h"

namespace std {
    omnn::math::SymmetricDouble abs(const omnn::math::SymmetricDouble& n)
    {
        return omnn::math::SymmetricDouble(abs(n.AsDouble()));
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

