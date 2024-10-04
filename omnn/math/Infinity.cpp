#include "Infinity.h"

using namespace omnn;
using namespace math;

// INFINITY

constinit std::string_view Infinity::SerializationName = "∞";

Valuable Infinity::operator -() const {
    return MInfinity();
}

Valuable& Infinity::operator +=(const Valuable& v)
{
    if (v.IsMInfinity())
        IMPLEMENT;
    return *this;
}

Valuable& Infinity::operator *=(const Valuable& v)
{
    if (v.IsInt()) {
        if (v.ca() < 0) {
            Become(MInfinity());
        } else if(v.ca() == 0) {
            IMPLEMENT;
        } else {
            //Infinity
        }
    } else if (v.IsMInfinity()) {
        IMPLEMENT;
    } else if (v < constants::zero)
        Become(MInfinity());
        
    return *this;
}

Valuable& Infinity::operator /=(const Valuable& v)
{
    if (v.IsInfinity() || v.IsMInfinity())
        IMPLEMENT
    else if (v < constants::zero)
        Become(MInfinity());
    else if (v.IsZero())
        IMPLEMENT;
    return *this;
}

Valuable& Infinity::operator %=(const Valuable& v)
{
    IMPLEMENT
}

bool Infinity::operator <(const Valuable& v) const
{
    return false;
}

Valuable& Infinity::operator^=(const Valuable& v)
{
    if (v.IsZero())
        IMPLEMENT
    else if (!v.IsInt())
        IMPLEMENT
    else if (v < constants::zero)
        IMPLEMENT
    return *this;
}

Valuable& Infinity::d(const Variable& x)
{
    IMPLEMENT
}

// MINFINITY

constinit std::string_view MInfinity::SerializationName = "-∞";

Valuable MInfinity::operator -() const {
    return Infinity();
}

Valuable& MInfinity::operator +=(const Valuable& v)
{
    if (v.IsInfinity())
        IMPLEMENT
    return *this;
}

Valuable& MInfinity::operator *=(const Valuable& v)
{
    if (v.IsInfinity())
        IMPLEMENT
    else if (v < constants::zero)
        Become(Infinity());
    else if (v.IsZero())
        Become(NaN());
    return *this;
}

Valuable& MInfinity::operator /=(const Valuable& v)
{
    if (v.IsInfinity() || v.IsMInfinity())
        IMPLEMENT
    else if (v < constants::zero)
        Become(MInfinity());
    else if (v.IsZero())
        IMPLEMENT
    return *this;
}

Valuable& MInfinity::operator %=(const Valuable& v)
{
    IMPLEMENT
}

bool MInfinity::operator <(const Valuable& v) const
{
    if (v.IsMInfinity())
        IMPLEMENT;
    return true;
}

Valuable& MInfinity::operator^=(const Valuable& v)
{
    if (v.IsZero())
        IMPLEMENT
    else if (!v.IsInt())
        IMPLEMENT
    else if (v < constants::zero)
        IMPLEMENT
    return *this;
}

Valuable& MInfinity::d(const Variable& x)
{
    IMPLEMENT
}


// NaN

constinit std::string_view NaN::SerializationName = "NaN";
