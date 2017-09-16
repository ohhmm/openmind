//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Valuable.h"
#include "Integer.h"

namespace omnn{
namespace extrapolator {

static void implement()
{
    throw "Implement!";
}
    
#define IMPLEMENT { implement(); throw; }

    
Valuable::~Valuable()
{
}

Valuable Valuable::operator -(const self& v) const
{
    IMPLEMENT
}

Valuable& Valuable::operator +=(const Valuable& number)
{
    IMPLEMENT
}

Valuable& Valuable::operator *=(const Valuable& number)
{
    IMPLEMENT
}

Valuable& Valuable::operator /=(const Valuable& number)
{
    IMPLEMENT
}

Valuable& Valuable::operator %=(const Valuable& number)
{
    IMPLEMENT
}

Valuable& Valuable::operator--()
{
    if(exp)
        return exp->operator--();
    else
        IMPLEMENT
}
    
Valuable& Valuable::operator++()
{
    if(exp)
        return exp->operator++();
    else
        IMPLEMENT
}
 
bool Valuable::operator<(const Valuable& number) const
{
    IMPLEMENT
}

bool Valuable::operator==(const Valuable& number) const
{
    IMPLEMENT
}
    
std::ostream& operator<<(std::ostream& out, const Valuable& obj)
{
    auto i = dynamic_cast<const Integer*>(&obj);
    if(i)
        return out << *i;
    
    // no type matched
    IMPLEMENT
}


}}
