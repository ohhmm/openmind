#include "SymmetricDouble.h"

namespace std {
    omnn::math::SymmetricDouble abs(const omnn::math::SymmetricDouble& n)
    {
        return omnn::math::SymmetricDouble(abs(n.AsDouble()));
    }
    omnn::math::SymmetricDouble sqrt(const omnn::math::SymmetricDouble& n)
    {
        if(n.AsDouble()<0) return -sqrt(-n.AsDouble());
        else return sqrt(n.AsDouble());
    }
}
