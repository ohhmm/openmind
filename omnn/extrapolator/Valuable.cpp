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

Valuable Valuable::operator -() const
{
    if(exp)
        return exp->operator-();
    else
        IMPLEMENT
}

Valuable& Valuable::operator +=(const Valuable& v)
{
    if(exp)
        return exp->operator+=(v);
    else
        IMPLEMENT
}
    
Valuable& Valuable::operator +=(int v)
{
    if(exp)
        return exp->operator+=(v);
    else
        IMPLEMENT
    }

Valuable& Valuable::operator *=(const Valuable& v)
{
    if(exp)
        return exp->operator*=(v);
    else
        IMPLEMENT

}

Valuable& Valuable::operator /=(const Valuable& v)
{
    if(exp)
        return exp->operator/=(v);
    else
        IMPLEMENT

}

Valuable& Valuable::operator %=(const Valuable& v)
{
    if(exp)
        return exp->operator%=(v);
    else
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
 
bool Valuable::operator<(const Valuable& v) const
{
    if(exp)
        return exp->operator<(v);
    else
        IMPLEMENT

}

bool Valuable::operator==(const Valuable& v) const
{
    if(exp)
        return exp->operator==(v);
    else
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
