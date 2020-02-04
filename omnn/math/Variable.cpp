//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Variable.h"
#include "Exponentiation.h"
#include "Integer.h"
#include "Modulo.h"
#include "Product.h"
#include "Sum.h"
#include "VarHost.h"

namespace omnn{
namespace math {

    Variable::Variable()
    : varSetHost(&VarHost::Global<>(), [](auto){})
    , varId(VarHost::Global<>().NewVarId())
    {
        hash = varSetHost->Hash(varId);
        maxVaExp=1;
    }
    
    Variable::Variable(const Variable& v)
    : varSetHost(v.varSetHost)
    , varId(v.varSetHost->CloneId(v.varId))
    {
        hash = v.Hash();
        maxVaExp=1;
    }
    
    void Variable::SetId(boost::any id) {
        varId = id;
        hash = varSetHost->Hash(id);
    }
    
    Variable::Variable(VarHost::ptr varHost)
    : varSetHost(varHost)
    {
        if(!varHost)
            throw "the varHost is mandatory parameter";
        maxVaExp=1;
    }
    
    Valuable Variable::operator -() const
    {
        return Product{-1, *this};
    }
    
    Valuable& Variable::operator +=(const Valuable& v)
    {
        auto c = cast(v);
        if(c && *c==*this)
        {
            Become(Product{2, *this});
        }
        else
            Become(Sum{*this, v});

        return *this;
    }
    
    Valuable& Variable::operator *=(const Valuable& v)
    {
        if (Same(v)) {
            return Become(Exponentiation(*this, 2));
        }
        else if (Exponentiation::cast(v))
            return Become(v**this);
        return Become(Product{*this, v});
    }
    
    bool Variable::MultiplyIfSimplifiable(const Valuable& v)
    {
        auto is = v.IsVa();
        if (is) {
            is = operator==(v);
            if (is) {
                sq();
            }
        } else if (v.IsSimple()) {
        } else {
            auto s = v.IsMultiplicationSimplifiable(*this);
            is = s.first;
            if (is) {
                Become(std::move(s.second));
            }
        }
        return is;
    }

    std::pair<bool,Valuable> Variable::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is;
        is.first = v.IsVa() && operator==(v);
        if (is.first) {
            is.second = Sq();
        } else if (v.IsSimple()) {
        } else {
            is = v.IsMultiplicationSimplifiable(v);
        }
        return is;
    }

    bool Variable::SumIfSimplifiable(const Valuable& v)
    {
        std::pair<bool,Valuable> is;
        is.first = v.IsVa() && operator==(v);
        if (is.first) {
            Become(*this*2);
        } else {
            is = v.IsSumationSimplifiable(*this);
            if(is.first)
                Become(std::move(is.second));
        }
        return is.first;
    }

    std::pair<bool,Valuable> Variable::IsSumationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is;
        is.first = v.IsVa() && operator==(v);
        if (is.first) {
            is.second = v * 2;
        } else if (v.IsSimple()) {
        } else {
            is = v.IsSumationSimplifiable(*this);
            if(is.first)
            {
                if (is.second.Complexity() > v.Complexity())
                    IMPLEMENT;
            }
        }
        return is;
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

    Valuable& Variable::operator %=(const Valuable& v)
    {
        return Become(Modulo(*this, v));
    }

    Valuable& Variable::operator^=(const Valuable& v)
    {
        if(v.IsInt() && v == 0_v)
            Become(1_v);
        else
            Become(Exponentiation(*this, v));
        return *this;
    }
    
    Valuable& Variable::d(const Variable& x)
    {
        return Become(1_v);
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
        if (v.IsVa())
        {
            auto i = cast(v);
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
        auto mve = getMaxVaExp();
        auto vmve = v.getMaxVaExp();
        auto is = mve > vmve;
        if (mve != vmve)
        {}
        else if (v.IsVa())
            is = base::IsComesBefore(v);
        else if (v.IsProduct())
            is = Product{*this}.IsComesBefore(v);
        else
            is = !v.FindVa();
        return is;
    }
    
    void Variable::CollectVa(std::set<Variable>& s) const
    {
        s.insert(*this);
    }

    void Variable::CollectVaNames(std::map<std::string, Variable>& s) const{
        s[str()] = *this;
    }

    bool Variable::eval(const std::map<Variable, Valuable>& with) {
        auto it = with.find(*this);
        auto evaluated = it != with.end();
        if(evaluated) {
            auto c = it->second;
            Become(std::move(c));
        }
        return evaluated;
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
    
    Valuable Variable::InCommonWith(const Valuable& v) const
    {
        auto c = 1_v;
        if (v.IsProduct()) {
            c = v.InCommonWith(*this);
        } else if (v.IsExponentiation()) {
            auto e = Exponentiation::cast(v);
            if (e->getBase() == *this) {
                if (e->getExponentiation().IsInt()) {
                    if (e->getExponentiation() > 0) {
                        c = *this;
                    }
                } else {
                    IMPLEMENT
                }
            }
        } else if (v.IsVa()) {
            if (*this == v)
                c = v;
        } else if (v.IsInt() || v.IsSimpleFraction()) {
        } else {
            IMPLEMENT
        }
        return c;
    }
    
    Valuable Variable::operator()(const Variable& va, const Valuable& augmentation) const
    {
        if (*this == va) {
            return {augmentation};
        }
        else
            IMPLEMENT
    }

}}
