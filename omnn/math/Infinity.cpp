#include "Infinity.h"

using namespace omnn;
using namespace math;

// INFINITY

std::ostream& Infinity::print(std::ostream& out) const {
    return out << "∞";
}

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
    if (v.IsMInfinity()) {
        IMPLEMENT;
    }
    else if (v < 0_v)
        Become(MInfinity());
    else if (v == 0_v)
        IMPLEMENT;
    return *this;
}

Valuable& Infinity::operator /=(const Valuable& v)
{
    if (v.IsInfinity() || v.IsMInfinity())
        IMPLEMENT
    else if (v < 0_v)
        Become(MInfinity());
    else if (v == 0_v)
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
    if (v == 0_v)
        IMPLEMENT
    else if (!v.IsInt())
        IMPLEMENT
    else if (v < 0_v)
        IMPLEMENT
    return *this;
}

Valuable& Infinity::d(const Variable& x)
{
    IMPLEMENT
}

// MINFINITY

std::ostream& MInfinity::print(std::ostream& out) const {
    return out << "-∞";
}

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
    else if (v < 0_v)
        Become(MInfinity());
    else if (v == 0_v)
        IMPLEMENT
    return *this;
}

Valuable& MInfinity::operator /=(const Valuable& v)
{
    if (v.IsInfinity() || v.IsMInfinity())
        IMPLEMENT
    else if (v < 0_v)
        Become(MInfinity());
    else if (v == 0_v)
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
    if (v == 0_v)
        IMPLEMENT
    else if (!v.IsInt())
        IMPLEMENT
    else if (v < 0_v)
        IMPLEMENT
    return *this;
}

Valuable& MInfinity::d(const Variable& x)
{
    IMPLEMENT
}
