//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Integer.h"
#include "Fraction.h"
#include "Sum.h"

namespace omnn{
namespace extrapolator {

    Valuable Integer::operator -() const
    {
        return Integer(-arbitrary);
    }
    
    Valuable& Integer::operator +=(const Valuable& v)
    {
        auto i = cast(v);
        if (i)
            arbitrary += i->arbitrary;
        else
        {
            return Become(Sum(*this, v));
        }
        return *this;
    }

    Valuable& Integer::operator +=(int v)
    {
        arbitrary += v;
        return *this;
    }

    Valuable& Integer::operator *=(const Valuable& v)
    {
        auto i = cast(v);
        if (i)
            arbitrary *= i->arbitrary;
        else
        {
            // try other type
            auto i = Fraction::cast(v);
            if (i)
                Become(v**this);
            else
                // no type matched
				// TODO : 
                base::operator *=(v);
        }
        return *this;
    }

    Valuable& Integer::operator /=(const Valuable& v)
    {
        auto i = cast(v);
        if (i)
        {
            auto div = arbitrary/i->arbitrary;
            if(div*i->arbitrary==arbitrary)
                arbitrary=div;
            else
                Become(Fraction(*this,*i));
        }
        else
        {
			Become(Fraction(*this, v));
        }
        return *this;
    }

    Valuable& Integer::operator %=(const Valuable& v)
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

    Valuable& Integer::operator --()
    {
        arbitrary--;
        return *this;
    }

    Valuable& Integer::operator ++()
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
            auto i = Fraction::cast(v);
            if (i)
            {
                return v >= *this;
            }
            else
            {
                // try other type
                // no type matched
            }
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

    std::ostream& Integer::print(std::ostream& out) const
    {
        return out << arbitrary;
    }
    
    std::ostream& operator <<(std::ostream& out, const Integer& obj)
    {
        return obj.print(out);
    }

}}
