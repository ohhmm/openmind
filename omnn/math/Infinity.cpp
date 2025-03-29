#include "Infinity.h"
#include "NaN.h"
#include "Limit.h"

using namespace omnn;
using namespace math;

// INFINITY

constinit std::string_view Infinity::SerializationName = "inf";

Valuable Infinity::operator -() const {
    return MInfinity();
}

Valuable& Infinity::operator +=(const Valuable& v)
{
    if (v.IsMInfinity()) {
        Become(NaN());  // Infinity + (-Infinity) = NaN
    }
    return *this;
}

Valuable& Infinity::operator *=(const Valuable& v)
{
    if (v.IsInt()) {
        if (v.ca() < 0) {
            Become(MInfinity());
        } else if(v.ca() == 0) {
            Become(NaN());  // Infinity * 0 = NaN
        } else {
            // Stay infinity
        }
    } else if (v.IsMInfinity()) {
        Become(MInfinity());  // Infinity * (-Infinity) = -Infinity
    } else if (v < constants::zero) {
        Become(MInfinity());
    }
    return *this;
}

Valuable& Infinity::operator /=(const Valuable& v)
{
    if (v.IsInfinity() || v.IsMInfinity()) {
        Become(NaN());  // Infinity/Infinity or Infinity/(-Infinity) = NaN
    } else if (v < constants::zero) {
        Become(MInfinity());
    } else if (v.IsZero()) {
        LOG_AND_IMPLEMENT("Complex Infinity")
    }
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
        is.second = NaN();
    } else {
        is.second = *this;
    }
    return is;
}

std::pair<bool, Valuable> Infinity::IsMultiplicationSimplifiable(const Valuable& value) const {
    std::pair<bool, Valuable> is;
    is.first = value.IsSimple();
    if (is.first) {
        if (value < constants::zero) {
            is.second = MInfinity();
        } else if (value.IsZero()) {
            is.second = NaN();
        } else {
            is.second = *this;
        }
    }
    return is;
}

Valuable& Infinity::operator^=(const Valuable& v)
{
    if (v.IsZero()) {
        Become(NaN());
    } else if (!v.IsInt()) {
        IMPLEMENT
    } else if (v < constants::zero) {
        LOG_AND_IMPLEMENT("Limit->0")
    }
    return *this;
}

Valuable& Infinity::d(const Variable& x)
{
    IMPLEMENT
}

// MINFINITY

constinit std::string_view MInfinity::SerializationName = "-inf";

Valuable MInfinity::operator -() const {
    return Infinity();
}

Valuable& MInfinity::operator +=(const Valuable& v)
{
    if (v.IsInfinity()) {
        Become(NaN());  // -Infinity + Infinity = NaN
    }
    return *this;
}

std::pair<bool, Valuable> MInfinity::IsMultiplicationSimplifiable(const Valuable& value) const {
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
    if (v.IsInfinity()) {
        // -Infinity * Infinity = -Infinity
    } else if (v < constants::zero) {
        Become(Infinity());
    } else if (v.IsZero()) {
        Become(NaN());
    }
    return *this;
}

Valuable& MInfinity::operator /=(const Valuable& v)
{
    if (v.IsInfinity() || v.IsMInfinity()) {
        Become(NaN());  // -Infinity/Infinity or -Infinity/(-Infinity) = NaN
    } else if (v < constants::zero) {
        Become(MInfinity());
    } else if (v.IsZero()) {
        LOG_AND_IMPLEMENT("Complex Infinity")
    }
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
    if (v.IsZero()) {
        Become(NaN());
    } else if (!v.IsInt()) {
        IMPLEMENT
    } else if (v < constants::zero) {
        LOG_AND_IMPLEMENT("Limit->0")
    }
    return *this;
}

Valuable& MInfinity::d(const Variable& x)
{
    IMPLEMENT
}

