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

bool Infinity::operator <(const Valuable& value) const
{
    return {};
}

std::pair<bool, Valuable> omnn::math::Infinity::IsSummationSimplifiable(const Valuable& value) const {
    std::pair<bool, Valuable> is;
    is.first = true;
    if ((value.is_optimized() && value.IsMInfinity())
    || (!value.is_optimized() && value.Optimized().IsMInfinity())
    ) {
        is.second = NaN(); // FIXME: check if value is optimized
    } else {
        is.second = *this;
    }
    return is;
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

std::pair<bool, Valuable> omnn::math::MInfinity::IsMultiplicationSimplifiable(const Valuable& value) const {
    std::pair<bool, Valuable> is;
    is.first = value.IsSimple();
    if (is.first) {
        if (value < constants::zero)
            is.second = Infinity();
        else if (value.IsZero())
            is.second = NaN();
        else
            is.second = *this;
    }
    return is;
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
