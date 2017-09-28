//
// Created by Сергей Кривонос on 25.09.17.
//

#include "Variable.h"
#include "Product.h"
#include "Sum.h"

namespace omnn{
namespace extrapolator {

    Valuable Variable::operator -() const
    {
        return Valuable(Product(-1, *this).Clone());
    }
    
    Valuable& Variable::operator +=(const Valuable& v)
    {
        *static_cast<Valuable*>(this) = Valuable(Sum(*this, v).Clone());
        return *this;
    }
    
    Valuable& Variable::operator +=(int v)
    {
        *static_cast<Valuable*>(this) = Valuable(Sum(*this, v).Clone());
        return *this;
    }
    
    Valuable& Variable::operator *=(const Valuable& v)
    {
        *static_cast<Valuable*>(this) = Valuable(Product(*this, v).Clone());
        return *this;
    }
    
    Valuable& Variable::operator /=(const Valuable& v)
    {
        auto i = cast(v);
        if (i)
        {
            if (*this==*i) {
                Become(Integer(1));
            }
            else
                throw "Implement! Need fraction support for valuables";
        }
        else
        {
            throw "Implement! Need fraction support for valuables";
            auto i = Integer::cast(v);
            if (i)
            {
                // todo : Implement!
            }
            else
            {
                // try other type
                // no type matched
                base::operator /=(v);
            }
            
        }
        optimize();
        return *this;
    }
    
    Valuable& Variable::operator --()
    {
        return *this+=-1;
    }
    
    Valuable& Variable::operator ++()
    {
        return *this+=1;
    }
    
    bool Variable::operator <(const Valuable& v) const
    {
        auto i = cast(v);
        if (i)
        {
            if (&varSetHost != &i->varSetHost) {
                throw "Unable to compare variable sequence numbers from different var hosts. Do you need a lambda for delayed comparision during evaluation? implement then.";
            }
            return varSetHost.CompareIdsLess(varId, i->varId);
        }
        else
        {
            throw "Unable to compare variable sequence numbers from different var hosts. Do you need a lambda for delayed comparision during evaluation? implement then.";
        }
        
        // not implemented comparison to this Valuable descent
        return base::operator <(v);
    }
    
    bool Variable::operator ==(const Valuable& v) const
    {
        auto i = cast(v);
        if (i)
        {
            if (&varSetHost != &i->varSetHost) {
                throw "Unable to compare variable sequence numbers from different var hosts. Do you need a lambda for delayed comparision during evaluation? implement then.";
            }
            return varSetHost.CompareIdsEqual(varId, i->varId);
        }
        else
        {
            throw "Unable to compare variable sequence numbers from different var hosts. Do you need a lambda for delayed comparision during evaluation? implement then.";
        }
        
        // not implemented comparison to this Valuable descent
        return base::operator ==(v);
    }
    
    std::ostream& Variable::print(std::ostream& out) const
    {
        return varSetHost.print(out, varId);
    }


}}
