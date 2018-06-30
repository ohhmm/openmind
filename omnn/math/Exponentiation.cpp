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
        maxVaExp = getMaxVaExp();
    }
    
    a_int Exponentiation::getMaxVaExp() const
    {
        if (eexp.IsInt()) {
            a_int _ = static_cast<a_int>(eexp);
            _ *= ebase.getMaxVaExp();
            return _;
        } else if (eexp.FindVa()) {
            a_int i = ebase.getMaxVaExp();
            if (i) {
                auto _ = eexp;
                const Variable* v;
                while ((v = _.FindVa())) {
                    _.Eval(*v, 0);
                }
                _.optimize();
                i *= static_cast<a_int>(_);
            }
            return i;
        } else {
            auto maxVaExp = eexp * Integer(ebase.getMaxVaExp());
            if (maxVaExp.IsInt()) {
                return static_cast<a_int>(maxVaExp);
            } else if(!optimizations) {
                optimizations = true;
                maxVaExp.optimize();
                optimizations = {};
                if (maxVaExp.IsInt()) {
                    return static_cast<a_int>(maxVaExp);
                }
            }
        }
        
        IMPLEMENT
    }
    
	Valuable Exponentiation::operator -() const
    {
        return Product {*this, -1};
    }

    void Exponentiation::optimize()
    {
        if (optimized)
            return;

        if (!optimizations)
        {
            hash = ebase.Hash() ^ eexp.Hash();
            return;
        }

        ebase.optimize();
        eexp.optimize();

        if (ebase.IsExponentiation() || ebase.IsProduct())
        {
            ebase ^= eexp;
            Become(std::move(ebase));
            return;
        }
        
        if (eexp.IsSum())
        {
            auto s = Sum::cast(eexp);
            auto sz = s->size();
            auto v = 1_v;
            for(auto it = s->begin(), e = s->end();
                it != e; )
            {
                if (it->IsInt()) {
                    v *= ebase^*it;
                    s->Delete(it);
                }
                else
                    ++it;
            }
            if (sz != s->size()) {
                Become(*this * v);
                return;
            }
        }

        // todo : comment this and try System test
        if (ebase.IsSum() && eexp.IsFraction())
        {
            auto f = Fraction::cast(eexp);
            auto& d = f->getDenominator();
            if (d == ebase.getMaxVaExp()) {
                auto vars = ebase.Vars();
                if (vars.size() == 1) {
                    auto va = *vars.begin();
                    auto s = ebase(va);
                    if (s.size()==1) {
                        auto eq = va - *s.begin();
                        auto sq = eq ^ d;
                        auto check = ebase/sq;
                        if (check.IsInt() || check.IsSimpleFraction()) {
                            ebase = eq;
                            eexp = f->getNumerator();
                        } else {
                            IMPLEMENT
                        }
                    }
                }
            }
        }
        
        if (ebase.IsSum() && eexp.IsFraction())
        {
            auto f = Fraction::cast(eexp);
            auto& d = f->getDenominator();
            auto e = ebase ^ f->getNumerator();
            if (d == e.getMaxVaExp()) {
                auto vars = e.Vars();
                if (vars.size() == 1) {
                    auto va = *vars.begin();
                    auto s = e(va);
                    if (s.size()==1) {
                        auto eq = va - *s.begin();
                        auto sq = eq ^ d;
                        auto check = e/sq;
                        if (check.IsInt() || check.IsSimpleFraction()) {
                            Become(std::move(eq));
                            return;
                        } else {
                            IMPLEMENT
                        }
                    }
                }
            }
        }
        
//        if (eexp.IsFraction() && ebase.IsInt()) {
//            auto f = Fraction::cast(eexp);
//            if (f->IsSimple()) {

        
//            }
//        }

        if (ebase.IsFraction() && eexp.IsInt() && eexp < 0_v) {
            eexp = -eexp;
            ebase = Fraction::cast(ebase)->Reciprocal();
        }

        if (ebase.IsInt()) {
            if (ebase==1) {
                if (eexp.IsInt()
                    || (eexp.IsFraction() && !eexp.FindVa())
                    ) {
                    Become(std::move(ebase));
                    return;
                } else
                    IMPLEMENT;
            } else if (ebase==-1 && eexp.IsInt() && eexp > 0 && eexp!=1) {
                eexp = eexp.bit(0);
            }
        }

        bool ebz = ebase == 0_v;
        bool exz = eexp == 0_v;
        if(exz)
        {
            if(ebz)
                throw "NaN";

            Become(1_v);
            return;
        }
        else if(eexp == 1_v)
        {
            Become(std::move(ebase));
            return;
        }
        else if (ebz)
        {
            if (exz)
                throw "NaN";
            Become(0_v);
            return;
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
                        return;
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
                            } else if (eexp!=-1){
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
//                    else
//                    IMPLEMENT
                    break;
                }
                default:
                    IMPLEMENT
            }
        }

        if(IsExponentiation() && ebase.IsExponentiation())
        {
            auto e = cast(ebase);
            auto& eeexp = e->getExponentiation();
            if ((eeexp.FindVa() == nullptr) == (eexp.FindVa() == nullptr)) {
                eexp *= eeexp;
                // todo : copy if it shared
                ebase = std::move(const_cast<Valuable&>((e->getBase())));
            }
        }

        if (IsExponentiation()) {
            hash = ebase.Hash() ^ eexp.Hash();
            optimized = true;
            v.clear();
            if (ebase.IsVa())
            {
                auto vaBase = Variable::cast(ebase);
                v[*vaBase] = eexp;
            }
        }
    }
    
    Valuable& Exponentiation::operator +=(const Valuable& v)
    {
        return Become(Sum {*this, v});
    }

    Valuable& Exponentiation::operator *=(const Valuable& v)
    {
        const Exponentiation* e;
        const Fraction* f;
        if (v.IsExponentiation()
            && ebase == (e = cast(v))->ebase)
        {
            eexp += e->eexp;
            optimized={};
        }
        else if(v.IsFraction()
                && (f = Fraction::cast(v))->getDenominator() == ebase)
        {
            --eexp;
            optimized={};
            optimize();
            return *this *= f->getNumerator();
        }
        else if(ebase == v && v.FindVa())
        {
            ++eexp;
            optimized={};
        }
        else if(v.IsProduct())
        {
            return Become(v * *this);
        }
        else if(v.IsInt() && v==1)
        {
            return *this;
        }
        else
        {
            return Become(Product{*this, v});
        }

        optimize();
        return *this;
    }

    Valuable& Exponentiation::operator /=(const Valuable& v)
    {
        if (v.IsExponentiation())
        {
            auto e = cast(v);
            if(ebase == e->ebase)
            {
                eexp -= e->eexp;
            }
            else
            {
                Become(Fraction(*this, v));
                return *this;
            }
        }
        else if(v.IsFraction())
        {
            auto f = Fraction::cast(v);
            *this *= f->Reciprocal();
            return *this;
        }
        else if(ebase == v)
        {
            --eexp;
        }
        else
        {
            Become(Fraction(*this, v));
            return *this;
        }

        optimized={};
        optimize();
        return *this;
    }

    Valuable& Exponentiation::operator^=(const Valuable& v)
    {
        eexp *= v;
        optimized={};
        optimize();
        return *this;
    }
    
    Valuable& Exponentiation::d(const Variable& x)
    {
        optimized={};
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
    }

    std::ostream& Exponentiation::print(std::ostream& out) const
    {
        out << '(';
        if(!((ebase.IsInt() && ebase>=0) || ebase.IsVa() || ebase.IsSum()))
            out << '(' << ebase << ')';
        else
            out << ebase;
        out << '^';
        if(!(eexp.IsInt() || eexp.IsVa() || ebase.IsSum()))
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
        optimized={};
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
        auto mve = getMaxVaExp();
        auto vmve = v.getMaxVaExp();
        auto is = mve > vmve;
        if (mve != vmve)
        {}
        else if (v.IsExponentiation())
        {
            auto e = cast(v);
            bool baseIsVa = ebase.IsVa();
            bool vbaseIsVa = e->ebase.IsVa();
            if (baseIsVa && vbaseIsVa)
                is = eexp == e->eexp ? ebase.IsComesBefore(e->ebase) : eexp > e->eexp;
            else if(baseIsVa)
                is = false;
            else if(vbaseIsVa)
                is = true;
            else if(ebase == e->ebase)
                is = eexp.IsComesBefore(e->eexp);
            else if(eexp == e->eexp)
                is = ebase.IsComesBefore(e->ebase);
            else
            {
                is = ebase.IsComesBefore(e->ebase) || eexp.IsComesBefore(e->eexp); //  || str().length() > e->str().length();
//                auto expComesBefore = eexp.IsComesBefore(e->eexp);
//                auto ebaseComesBefore = ebase.IsComesBefore(e->ebase);
//                is = expComesBefore==ebaseComesBefore || str().length() > e->str().length();
            }
        }
        else if(v.IsProduct())
        {
            is = Product{*this}.IsComesBefore(v);
        }
        else if(v.IsInt())
            is = true;
//        else if(v.IsFraction())
//        {is=}
        else if(v.IsVa())
            is = !!FindVa();
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

    Valuable & Exponentiation::sq()
    {
        eexp *= 2;
        optimized = {};
        optimize();
        return *this;
    }

    const Valuable::vars_cont_t& Exponentiation::getCommonVars() const
    {
        return v;
    }
    
    Valuable::solutions_t Exponentiation::operator()(const Variable& v, const Valuable& augmentation) const
    {
        if (!eexp.FindVa() && eexp!=0 && augmentation==0) {
            return ebase(v,augmentation);
        } else if (eexp.IsSimpleFraction()) {
            auto f = Fraction::cast(eexp);
            return (ebase^f->getNumerator())(v,augmentation^f->getDenominator());
        } else {
            IMPLEMENT
        }
    }
}}
