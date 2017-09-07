//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Valuable.h"
#include "Integer.h"

namespace omnn{
namespace extrapolator {

Valuable::~Valuable()
{
}

Valuable Valuable::operator -(const self& v) const
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

Valuable& Valuable::operator--()
{
    throw "Implement!";
}
    
Valuable& Valuable::operator++()
{
    throw "Implement!";
}
 
bool Valuable::operator<(const Valuable& number) const
{
    throw "Implement!";
}

bool Valuable::operator==(const Valuable& number) const
{
    throw "Implement!";
}
    
std::ostream& operator<<(std::ostream& out, const Valuable& obj)
{
    auto i = dynamic_cast<const Integer*>(&obj);
    if(i)
        return out << *i;
    // no type matched
    throw "Implement!";
}


}}
