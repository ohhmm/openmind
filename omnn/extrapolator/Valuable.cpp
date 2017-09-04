//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Valuable.h"

namespace omnn{
namespace extrapolator {

Valuable::~Valuable()
{
}

Valuable Valuable::operator -(const self& v)
{
    throw "Implement!";
}

Valuable& Valuable::operator +=(const Valuable& number)
{
    throw "Implement!";
}

Valuable& Valuable::operator *=(const Valuable& number)
{
    throw "Implement!";
}

Valuable& Valuable::operator /=(const Valuable& number)
{
    throw "Implement!";
}

Valuable& Valuable::operator %=(const Valuable& number)
{
    throw "Implement!";
}

Valuable Valuable::operator--(int)
{
    throw "Implement!";
}
}}
