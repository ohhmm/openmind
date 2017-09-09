//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Integer.h"


namespace omnn{
namespace extrapolator {

Valuable Integer::operator -(const Valuable& v) const
{
    return Integer(base_int(0) - v);
}
    
omnn::extrapolator::Valuable& Integer::operator +=(const Valuable& v)
{
    auto i = dynamic_cast<const Integer*>(&v);
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

omnn::extrapolator::Valuable& Integer::operator *=(const Valuable& v)
{
    auto i = dynamic_cast<const Integer*>(&v);
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
    auto i = dynamic_cast<const Integer*>(&v);
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
    auto i = dynamic_cast<const Integer*>(&v);
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
    Valuable* vp = const_cast<Valuable*>(&v);
    Integer* ip = dynamic_cast<Integer*>(vp);
    if (ip)
        return arbitrary < ip->arbitrary;
    else
    {
        // try other type
    }
    
    
    if (v.getEncapsulatedInstance().empty()) {
        // not implemented comparison to this Valuable descent
        return base::operator <(v);
    }
    else
    {
        // encapsulated Valuable descent
        if(v.getEncapsulatedInstance().type() == typeid(Integer))
        {
            return arbitrary < getEncapsulatedInstance(v)->arbitrary;
        }
    }
    
    
}

bool Integer::operator ==(const Valuable& v) const
{
    auto i = dynamic_cast<const Integer*>(&v);
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
