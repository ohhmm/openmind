//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Variable.h"
#include "Exponentiation.h"
#include "Integer.h"
#include "Product.h"
#include "Sum.h"

namespace omnn{
namespace math {

    Valuable Variable::operator -() const
    {
        return Valuable(Product(-1, *this).Clone());
    }
    
    Valuable& Variable::operator +=(const Valuable& v)
    {
        auto c = cast(v);
        if(c && *c==*this)
        {
            return Become(Product(2, *this));
        }

        return Become(Sum(*this, v));
    }
    
    Valuable& Variable::operator +=(int v)
    {
        return Become(Sum(*this, v));
    }
    
    Valuable& Variable::operator *=(const Valuable& v)
    {
        if (Same(v)) {
            return Become(Exponentiation(*this, 2));
        }
        else if (Exponentiation::cast(v))
            return Become(v**this);
        return Become(Product(*this, v));
    }
    
    Valuable& Variable::operator /=(const Valuable& v)
    {
        auto i = cast(v);
        if (i && *this==*i)
        {
            Become(Integer(1));
        }
        else
        {
            *this *= 1 / v;
        }

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
    
    Valuable& Variable::operator^=(const Valuable& v)
    {
        auto ie = Integer::cast(v);
        if(ie && *ie == 0)
        {
            Become(1_v);
        }
        else
        {
            Become(Exponentiation(*this, v));
        }
        
        optimize();
        return *this;
    }
    
    bool Variable::operator <(const Valuable& v) const
    {
        auto i = cast(v);
        if (i)
        {
            if (varSetHost != i->varSetHost) {
                throw "Unable to compare variable sequence numbers from different var hosts. Do you need a lambda for delayed comparision during evaluation? implement then.";
            }
            return varSetHost->CompareIdsLess(varId, i->varId);
        }
        
        // not implemented comparison to this Valuable descent
        return base::operator <(v);
    }
    
    bool Variable::operator ==(const Valuable& v) const
    {
        auto i = cast(v);
        if (i)
        {
            if (varSetHost != i->varSetHost) {
                throw "Unable to compare variable sequence numbers from different var hosts. Do you need a lambda for delayed comparision during evaluation? implement then.";
            }
            return hash == v.Hash()
                && varSetHost->CompareIdsEqual(varId, i->varId);
        }
        else
        {   // compare with non-va
            return false;
        }
        
        // not implemented comparison to this Valuable descent
        return base::operator ==(v);
    }
    
    std::ostream& Variable::print(std::ostream& out) const
    {
        return varSetHost->print(out, varId);
    }
    
    bool Variable::IsComesBefore(const Valuable& v) const
    {
        return (Product::cast(v))
            ? Product(1,*this).IsComesBefore(v)
            : base::IsComesBefore(v);
    }
    
    void Variable::CollectVa(std::set<Variable>& s) const
    {
        s.insert(*this);
    }
    
    void Variable::Eval(const Variable& va, const Valuable& v)
    {
        if(va==*this)
        {
            auto copy = v;
            Become(std::move(copy));
        }
    }
    
    const Valuable::vars_cont_t& Variable::getCommonVars() const
    {
        vars[*this] = 1_v;
        if(vars.size()>1)
        {
            vars.clear();
            getCommonVars();
        }
        return vars;
    }
}}
