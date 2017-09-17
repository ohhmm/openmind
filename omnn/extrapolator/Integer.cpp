//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Integer.h"


namespace omnn{
namespace extrapolator {

Valuable Integer::operator -() const
{
    return Integer(-arbitrary);
}
    
omnn::extrapolator::Valuable& Integer::operator +=(const Valuable& v)
{
    auto i = cast(v);
    if (i)
    arbitrary += i->arbitrary;
    else
    {
        // try other type
        // no type matched
        base::operator +=(v);
    }
    return *this;
}

omnn::extrapolator::Valuable& Integer::operator +=(int v)
{
    arbitrary += v;
    return *this;
}

omnn::extrapolator::Valuable& Integer::operator *=(const Valuable& v)
{
    auto i = cast(v);
    if (i)
        arbitrary *= i->arbitrary;
    else
    {
        // try other type
        // no type matched
        base::operator *=(v);
    }
    return *this;
}

omnn::extrapolator::Valuable& Integer::operator /=(const Valuable& v)
{
    auto i = cast(v);
    if (i)
    arbitrary += i->arbitrary;
    else
    {
        // try other type
        // no type matched
        base::operator /=(v);
    }
    return *this;
}

omnn::extrapolator::Valuable& Integer::operator %=(const Valuable& v)
{
    auto i = cast(v);
    if (i)
    arbitrary %= i->arbitrary;
    else
    {
        // try other type
        // no type matched
        base::operator %=(v);
    }
    return *this;
}

omnn::extrapolator::Valuable& Integer::operator --()
{
    arbitrary--;
    return *this;
}

omnn::extrapolator::Valuable& Integer::operator ++()
{
    arbitrary++;
    return *this;
}

bool Integer::operator <(const Valuable& v) const
{
    auto i = cast(v);
    if (i)
        return arbitrary < i->arbitrary;
    else
    {
        // try other type
    }

    // not implemented comparison to this Valuable descent
    return base::operator <(v);
}

bool Integer::operator ==(const Valuable& v) const
{
    auto i = cast(v);
    if (i)
    return arbitrary == i->arbitrary;
    else
    {
        // try other type
    }
    // no type matched
    return base::operator ==(v);
}

std::ostream& operator <<(std::ostream& out, const Integer& obj)
{
    out << obj.arbitrary;
    return out;
}

}}
