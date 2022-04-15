#include "PrincipalSurd.h"

namespace omnn::math{

PrincipalSurd::PrincipalSurd(const Valuable& radicand, const Valuable& index)
    : base(radicand, index)
{

}


std::ostream &omnn::math::PrincipalSurd::print_sign(std::ostream &out) const
{
    if(_2 == 2)
        return out << "sqrt(" << _1 << ')';
    else
        return out << '(' << _2 << 'r' << _1 << ')';
}

}
