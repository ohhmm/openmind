#include "SymmetricDouble.h"

#include <cmath>
#include <string>


namespace std {

    omnn::math::SymmetricDouble sqrt(const omnn::math::SymmetricDouble& n)
    {
        return omnn::math::SymmetricDouble{
            n.AsDouble() < 0
            ? -sqrt(-n.AsDouble())
            : sqrt(n.AsDouble())
        };
    }

} // namespace std

