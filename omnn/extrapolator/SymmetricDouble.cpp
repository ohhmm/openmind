#include "SymmetricDouble.h"

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
