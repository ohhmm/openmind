//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Exponentiation.h"
#include "Accessor.h"
#include "Integer.h"
#include "Sum.h"
#include "Product.h"


namespace omnn{
namespace extrapolator {
    
    Exponentiation::Exponentiation(const Valuable& b, const Valuable& e)
    : ebase(b), eexp(e)
    {
        hash = ebase.Hash() ^ eexp.Hash();
        auto ie = Integer::cast(e);
        if(ie)
        {
            maxVaExp = boost::numeric_cast<decltype(maxVaExp)>(ie->operator Integer::const_base_int_ref());
        }
        else
        {
            IMPLEMENT
        }
    }
    
    Valuable Exponentiation::getBaseAccessor()
    {
        return Accessor(ebase, hash);
    }
    
    Valuable Exponentiation::getExponentiationAccessor()
    {
        return Accessor(eexp, hash);
    }
    
    int Exponentiation::getMaxVaExp() const
    {
        auto vaBase = Variable::cast(ebase);
        if (vaBase) {
            auto i = Integer::cast(getExponentiation());
            return static_cast<int>(*i);
        }
        if(eexp.FindVa())
            IMPLEMENT
        return 0;
    }
    
	Valuable Exponentiation::operator -() const
    {
        return Product(*this, -1);;
    }

    void Exponentiation::optimize()
    {
//        if (!optimizations)
//        {
//            hash = ebase.Hash() ^ eexp.Hash();
//            return;
//        }
        
        ebase.optimize();
        eexp.optimize();

        bool ebz = ebase == 0_v;
        bool exz = eexp == 0_v;
        if(exz)
        {
            if(ebz)
                throw "NaN";

            Become(1_v);
        }
        else if(eexp == 1_v)
        {
            Become(std::move(ebase));
        }
        else if (ebz)
        {
            if (exz)
                throw "NaN";
            Become(0_v);
        }
        else
        {
            auto ibase = Integer::cast(ebase);
            auto iexp = Integer::cast(eexp);
            if(ibase && iexp)
            {
                Become(*ibase ^ *iexp);
            }
        }
        auto e = cast(*this);
        if(e)
        {
            e = cast(e->ebase);
            if (e)
            {
                auto& eeexp = e->getExponentiation();
                if ((eeexp.FindVa() == nullptr) == (eexp.FindVa() == nullptr)) {
                    eexp *= eeexp;
                    // todo : copy if it shared
                    ebase = std::move(const_cast<Valuable&>((e->getBase())));
                }
            }
        
            if (cast(*this)) {
                hash = ebase.Hash() ^ eexp.Hash();
            }
        }
    }
    
    Valuable& Exponentiation::operator +=(const Valuable& v)
    {
        return Become(Sum(*this, v));
    }

    Valuable& Exponentiation::operator *=(const Valuable& v)
    {
        auto e = cast(v);
        if (e)
        {
            if(ebase == e->ebase)
            {
                eexp += e->eexp;
            }
            else
            {
                Become(Product(*this, v));
            }
        }
        else
        {
            auto f = Fraction::cast(v);
            if(f && f->getDenominator() == ebase)
            {
                --eexp;
                return operator*=(f->getNumerator());
            }

            if(ebase == v)
            {
                ++eexp;
            }
            else
            {
                Become(Product(*this, v));
            }
        }

        optimize();
        return *this;
    }

    Valuable& Exponentiation::operator /=(const Valuable& v)
    {
        auto e = cast(v);
        if (e)
        {
            if(ebase == e->ebase)
            {
                eexp -= e->eexp;
            }
            else
            {
                Become(Fraction(*this, v));
            }
        }
        else
        {
            auto f = Fraction::cast(v);
            if(f)
            {
                return operator*=(f->Reciprocal());
            }

            if(ebase == v)
            {
                --eexp;
            }
            else
            {
                Become(Fraction(*this, v));
            }
        }

        optimize();
        return *this;
    }

    Valuable& Exponentiation::operator --()
    {
        return *this+=-1_v;
    }

    Valuable& Exponentiation::operator ++()
    {
        return *this+=1_v;
    }

    bool Exponentiation::operator <(const Valuable& v) const
    {
        auto e = cast(v);
        if (e)
        {
            if (e->ebase == ebase)
                return eexp < e->eexp;
            if (e->eexp == eexp)
                return ebase < e->ebase;
        }
        
        return base::operator <(v);
    }

    bool Exponentiation::operator ==(const Valuable& v) const
    {
        if (hash == v.Hash())
        {
            auto e = cast(v);
            return e
                && ebase.Hash() == e->ebase.Hash()
                && eexp.Hash() == eexp.Hash()
                && ebase == e->ebase
                && eexp == e->eexp;
        }
        else
        {
            return false;
        }
        // no type matched
        return base::operator ==(v);
    }

    std::ostream& Exponentiation::print(std::ostream& out) const
    {
        auto noNeedBraces = [](auto& i) {
            return Integer::cast(i) || Variable::cast(i);
        };
        out << '(';
        if(!noNeedBraces(ebase))
            out << '(' << ebase << ')';
        else
            out << ebase;
        out << '^';
        if(!noNeedBraces(eexp))
            out << '(' << eexp << ')';
        else
            out << eexp;
        out << ')';
        return out;
    }

    const Variable* Exponentiation::FindVa() const
    {
        auto va = eexp.FindVa();
        return va ? va : ebase.FindVa();
    }

    void Exponentiation::Eval(const Variable& va, const Valuable& v)
    {
        ebase.Eval(va, v);
        eexp.Eval(va, v);
        hash = ebase.Hash() ^ eexp.Hash();
//        optimize();
    }
    
    void Exponentiation::CollectVa(std::set<Variable>& s) const
    {
        ebase.CollectVa(s);
        eexp.CollectVa(s);
    }
    
    bool Exponentiation::IsComesBefore(const Valuable& v) const
    {
        auto e = cast(v);
        if (e)
        {
            auto vaBase = Variable::cast(ebase);
            auto vbase = Variable::cast(e->ebase);
            if (vaBase && vbase)
            {
                return eexp == e->eexp ? *vaBase < *vbase : eexp > e->eexp;
            }
            else if(vaBase)
                return false;
            else if(vbase)
                return true;
            else
            {
                return *this > *e;
            }
        }

        return base::IsComesBefore(v);
    }
}}
