//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Exponentiation.h"
#include "Integer.h"
#include "Sum.h"
#include "Product.h"


namespace omnn{
namespace math {
    
    Exponentiation::Exponentiation(const Valuable& b, const Valuable& e)
    : ebase(b), eexp(e)
    {
        hash = ebase.Hash() ^ eexp.Hash();
        auto ie = Integer::cast(e);
        if(ie)
        {
            maxVaExp = ie->operator Integer::const_base_int_ref();
        }
        else
        {
            IMPLEMENT
        }
    }
    
    a_int Exponentiation::getMaxVaExp() const
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
        return Product {*this, -1};
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
            switch(view)
            {
                case View::None: {
                    auto iexp = Integer::cast(eexp);
                    if (iexp) {
                        auto b = 1_v;
                        for (; eexp--;) {
                            b *= ebase;
                        }
                        Become(std::move(b));
                        return;
                    }
                    auto ibase = Integer::cast(ebase);
                    if (ibase && iexp) {
                        Become(*ibase ^ *iexp);
                    }
                    break;
                }
                case View::Flat: {

                    if(eexp.IsInt())
                    {
                        if (ebase.IsVa()) {
                            break;
                        }
                        if (eexp != 0_v) {
                            if (eexp > 1) {
                                Valuable x = ebase;
                                Valuable n = eexp;
                                if (n < 0_v)
                                {
                                    x = 1_v / x;
                                    n = -n;
                                }
                                if (n == 0_v)
                                {
                                    Become(1_v);
                                    return;
                                }
                                auto y = 1_v;
                                while(n > 1)
                                {
                                    bool isInt = n.IsInt();
                                    if (!isInt)
                                        IMPLEMENT
                                    if (isInt && (n % 2_v) == 0_v)
                                    {
                                        x *= x;
                                        n /= 2;
                                    }
                                    else
                                    {
                                        y *= x;
                                        x *= x;
                                        --n;
                                        n /= 2;
                                    }
                                }
                                x *= y;
                                Become(std::move(x));
                            } else {
                                // negative
                                Become(1_v/(ebase^(-eexp)));
                            }
                        }
                        else { // zero
                            if (ebase == 0_v)
                            {
                                IMPLEMENT
                                throw "NaN"; // feel free to handle this properly
                            }
                            else
                            {
                                Become(1_v);
                            }
                        }
                    }
                    else
                    IMPLEMENT
                    break;
                }
                default:
                    IMPLEMENT
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
        return Become(Sum {*this, v});
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
                Become(Product {*this, v});
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
                Become(Product{*this, v});
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
    
    Valuable& Exponentiation::d(const Variable& x)
    {
        bool bhx = ebase.HasVa(x);
        bool ehx = eexp.HasVa(x);
        if(ehx) {
            IMPLEMENT
            if(bhx){
                
            }else{
                
            }
        } else if (bhx) {
            if(ebase == x)
                Become(eexp * (ebase ^ (eexp-1)));
            else
                IMPLEMENT
        } else
            Become(0_v);
        optimize();
        return *this;
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
        out << '(';
        if(!(ebase.IsInt() || ebase.IsVa()))
            out << '(' << ebase << ')';
        else
            out << ebase;
        out << '^';
        if(!(eexp.IsInt() || eexp.IsVa()))
            out << '(' << eexp << ')';
        else
            out << eexp;
        out << ')';
        return out;
    }

    std::ostream& Exponentiation::code(std::ostream& out) const
    {
        if(!eexp.IsInt())
            IMPLEMENT;

        out << "(1";
        for (auto i=eexp; i-->0;) {
            out << '*' << ebase;
        }
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
        bool is = getMaxVaExp() > v.getMaxVaExp();
        auto e = cast(v);
        if (e)
        {
            auto vaBase = Variable::cast(ebase);
            auto vbase = Variable::cast(e->ebase);
            if (vaBase && vbase)
                is = eexp == e->eexp ? *vaBase < *vbase : eexp > e->eexp;
            else if(vaBase)
                is = false;
            else if(vbase)
                is = true;
            else if(ebase == e->ebase)
                is = eexp.IsComesBefore(e->eexp);
            else if(eexp == e->eexp)
                is = ebase.IsComesBefore(e->ebase);
            else
            {
                auto expComesBefore = eexp.IsComesBefore(e->eexp);
                auto ebaseComesBefore = ebase.IsComesBefore(e->ebase);
                is = expComesBefore==ebaseComesBefore || *this > *e;
            }
        }
        else if(Product::cast(v))
        {
            is = Product{*this}.IsComesBefore(v);
        }
        else
            IMPLEMENT

        return is;
    }
    
    Valuable Exponentiation::calcFreeMember() const
    {
        Valuable c;
        if(ebase.IsSum() && eexp.IsInt()){
            c = ebase.calcFreeMember() ^ eexp;
        } else if(ebase.IsVa()) {
            c = 0_v;
        } else
            IMPLEMENT;
        return c;
    }

    const Valuable::vars_cont_t& Exponentiation::getCommonVars() const
    {
        v.clear();
        auto vaBase = Variable::cast(ebase);
        if(vaBase)
            v[*vaBase] = eexp;
        return v;
    }
}}
