//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Exponentiation.h"

namespace omnn{
namespace math {

Valuable& ValuableDescendantBase::operator^=(const Valuable& v)
{
    return Become(Exponentiation(*this,v));
}

}}
