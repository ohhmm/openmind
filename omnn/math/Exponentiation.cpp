//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Exponentiation.h"

#include "e.h"
#include "i.h"
#include "π.h"
#include "Integer.h"
#include "Sum.h"
#include "Product.h"


namespace omnn{
namespace math {

    a_int Exponentiation::getMaxVaExp(const Valuable& b, const Valuable& e)
    {
        if (e.IsInt()) {
            a_int _ = e.ca();
            _ *= b.getMaxVaExp();
            return _;
        } else if (e.FindVa()) {
            a_int i = b.getMaxVaExp();
            if (i) {
                auto _ = e;
                const Variable* v;
                while ((v = _.FindVa())) {
                    _.Eval(*v, 0);
                }
                _.optimize();
                i *= static_cast<a_int>(_);
            }
            return i;
        } else {
            auto maxVaExp = e * Integer(b.getMaxVaExp());
            if (maxVaExp.IsInt()) {
                return maxVaExp.ca();
            } else if(!optimizations) {
                optimizations = true;
                maxVaExp.optimize();
                optimizations = {};
                if (maxVaExp.IsInt()) {
                    return maxVaExp.ca();
                }
            }
        }

        IMPLEMENT
    }

    a_int Exponentiation::getMaxVaExp() const
    {
        return getMaxVaExp(getBase(), getExponentiation());
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
            hash = ebase().Hash() ^ eexp().Hash();
            return;
        }

        ebase().optimize();
        eexp().optimize();

        if (ebase().IsExponentiation() || ebase().IsProduct())
        {
            ebase() ^= eexp();
            Become(std::move(ebase()));
            return;
        }
        
        if (eexp().IsSum())
        {
            auto s = Sum::cast(eexp());
            auto sz = s->size();
            auto v = 1_v;
            for(auto it = s->begin(), e = s->end();
                it != e; )
            {
                if (it->IsInt()) {
                    v *= ebase()^*it;
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
        if (ebase().IsSum() && eexp().IsFraction())
        {
            auto f = Fraction::cast(eexp());
            auto& d = f->getDenominator();
            if (d == ebase().getMaxVaExp()) {
                auto vars = ebase().Vars();
                if (vars.size() == 1) {
                    auto va = *vars.begin();
                    auto eq = va - ebase()(va);
                    auto sq = eq ^ d;
                    auto check = ebase()/sq;
                    if (check.IsInt() || check.IsSimpleFraction()) {
                        ebase() = eq;
                        eexp() = f->getNumerator();
                    } else {
                        IMPLEMENT
                    }
                }
            }
        }
        
        if (ebase().IsSum() && eexp().IsFraction())
        {
            auto f = Fraction::cast(eexp());
            auto& d = f->getDenominator();
            auto e = ebase() ^ f->getNumerator();
            if (d == e.getMaxVaExp()) {
                auto vars = e.Vars();
                if (vars.size() == 1) {
                    auto va = *vars.begin();
                    auto eq = va - e(va);
                    auto sq = eq ^ d;
                    auto check = e / sq;
                    if (check.IsInt() || check.IsSimpleFraction()) {
                        Become(std::move(eq));
                        return;
                    } else {
                        IMPLEMENT
                    }
                }
            }
        }
        
//        if (eexp().IsFraction() && ebase().IsInt()) {
//            auto f = Fraction::cast(eexp());
//            if (f->IsSimple()) {

        
//            }
//        }

        if (ebase().IsFraction() && eexp().IsInt() && eexp() < 0_v) {
            eexp() = -eexp();
            ebase() = Fraction::cast(ebase())->Reciprocal();
        }

        if (ebase().Is_e()) {
            if (eexp().IsProduct()) {
                auto p = Product::cast(eexp());
                if (p->Has(constant::π) && p->Has(constant::i)) { // TODO : sequence does metter
//                    ebase() = -1;
//                    eexp() /= constant::i;
//                    eexp() /= constant::π;
                }
            }
        }

        if (ebase().IsInt()) {
            if (ebase()==1) {
                if (eexp().IsInt()
                    || (eexp().IsFraction() && !eexp().FindVa())
                    ) {
                    Become(std::move(ebase()));
                    return;
                } else
                    IMPLEMENT;
            } else if (ebase()==-1 && eexp().IsInt() && eexp() > 0 && eexp()!=1) {
                eexp() = eexp().bit(0);
            }
        }

        bool ebz = ebase() == 0_v;
        bool exz = eexp() == 0_v;
        if(exz)
        {
            if(ebz)
                throw "NaN";

            Become(1_v);
            return;
        }
        else if(eexp() == 1_v)
        {
            Become(std::move(ebase()));
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
                    auto iexp = Integer::cast(eexp());
                    if (iexp) {
                        auto b = 1_v;
                        for (; eexp()--;) {
                            b *= ebase();
                        }
                        Become(std::move(b));
                        return;
                    }
                    auto ibase = Integer::cast(ebase());
                    if (ibase && iexp) {
                        Become(*ibase ^ *iexp);
                        return;
                    }
                    break;
                }
                case View::Flat: {

                    if(eexp().IsInt())
                    {
                        if (ebase().IsVa()) {
                            break;
                        }
                        if (eexp() != 0_v) {
                            if (eexp() > 1) {
                                Valuable x = ebase();
                                Valuable n = eexp();
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
                                        x.sq();
                                        n /= 2;
                                    }
                                    else
                                    {
                                        y *= x;
                                        x.sq();
                                        --n;
                                        n /= 2;
                                    }
                                }
                                x *= y;
                                Become(std::move(x));
                            } else if (eexp()!=-1){
                                // negative
                                Become(1_v/(ebase()^(-eexp())));
                            }
                        }
                        else { // zero
                            if (ebase() == 0_v)
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

        if(IsExponentiation() && ebase().IsExponentiation())
        {
            auto e = cast(ebase());
            auto& eeexp = e->getExponentiation();
            if ((eeexp.FindVa() == nullptr) == (eexp().FindVa() == nullptr)) {
                eexp() *= eeexp;
                // todo : copy if it shared
                ebase() = std::move(const_cast<Valuable&>((e->getBase())));
            }
        }

        if (IsExponentiation()) {
            hash = ebase().Hash() ^ eexp().Hash();
            optimized = true;
            v.clear();
            if (ebase().IsVa())
            {
                auto vaBase = Variable::cast(ebase());
                v[*vaBase] = eexp();
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
        const Product* fdn = {};
        const Exponentiation* fdne;
        auto isProdHasExpWithSameBase = [this](const Product* p) -> const Exponentiation*
        {
            for(auto& it : *p){
                if (it.IsExponentiation()) {
                    auto e = Exponentiation::cast(it);
                    if (ebase() == e->getBase()) {
                        return e;
                    }
                }
            }
            return {};
        };
        if (v.IsExponentiation()
            && ebase() == (e = cast(v))->getBase())
        {
            eexp() += e->getExponentiation();
            optimized={};
        }
        else if(v.IsFraction()
                && (f = Fraction::cast(v))->getDenominator() == ebase())
        {
            --eexp();
            optimized={};
            optimize();
            return *this *= f->getNumerator();
        }
        else if(v.IsFraction()
                && f->getDenominator().IsProduct()
                && (fdn = Product::cast(f->getDenominator()))->Has(ebase()))
        {
            --eexp();
            optimized={};
            optimize();
            return *this *= f->getNumerator() / (*fdn / ebase());
        }
        else if(fdn
                && (fdne = isProdHasExpWithSameBase(fdn)))
        {
            eexp() -= fdne->getExponentiation();
            optimized={};
            optimize();
            return *this *= f->getNumerator() / (*fdn / *fdne);
        }
        else if(ebase() == v && v.FindVa())
        {
            ++eexp();
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
            return Become(Product{v, *this});
        }

        optimize();
        return *this;
    }

    Valuable& Exponentiation::operator /=(const Valuable& v)
    {
        if (v.IsExponentiation())
        {
            auto e = cast(v);
            if(ebase() == e->getBase())
            {
                eexp() -= e->getExponentiation();
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
        else if(ebase() == v)
        {
            --eexp();
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
        eexp() *= v;
        optimized={};
        optimize();
        return *this;
    }
    
    Valuable& Exponentiation::d(const Variable& x)
    {
        optimized={};
        bool bhx = ebase().HasVa(x);
        bool ehx = eexp().HasVa(x);
        if(ehx) {
            IMPLEMENT
            if(bhx){
                
            }else{
                
            }
        } else if (bhx) {
            if(ebase() == x)
                Become(eexp() * (ebase() ^ (eexp()-1)));
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
            if (e->getBase() == getBase())
                return getExponentiation() < e->getExponentiation();
            if (e->getExponentiation() == getExponentiation())
                return getBase() < e->getBase();
        }
        
        return base::operator <(v);
    }

    std::ostream& Exponentiation::print_sign(std::ostream& out) const
    {
        return out << "^";
    }

    std::ostream& Exponentiation::code(std::ostream& out) const
    {
        if(!getExponentiation().IsInt())
            IMPLEMENT;

        out << "(1";
        for (auto i=getExponentiation(); i-->0;) {
            out << '*' << getBase();
        }
        out << ')';
        
        return out;
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
            bool baseIsVa = getBase().IsVa();
            bool vbaseIsVa = e->getBase().IsVa();
            if (baseIsVa && vbaseIsVa)
                is = getExponentiation() == e->getExponentiation() ? getBase().IsComesBefore(e->getBase()) : getExponentiation() > e->getExponentiation();
            else if(baseIsVa)
                is = false;
            else if(vbaseIsVa)
                is = true;
            else if(getBase() == e->getBase())
                is = getExponentiation().IsComesBefore(e->getExponentiation());
            else if(getExponentiation() == e->getExponentiation())
                is = getBase().IsComesBefore(e->getBase());
            else
            {
                is = getBase().IsComesBefore(e->getBase()) || getExponentiation().IsComesBefore(e->getExponentiation()); //  || str().length() > e->str().length();
//                auto expComesBefore = eexp().IsComesBefore(e->eexp());
//                auto ebase()ComesBefore = ebase().IsComesBefore(e->ebase());
//                is = expComesBefore==ebase()ComesBefore || str().length() > e->str().length();
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
        if(getBase().IsSum() && getExponentiation().IsInt()){
            c = getBase().calcFreeMember() ^ getExponentiation();
        } else if(getBase().IsVa()) {
            c = 0_v;
        } else
            IMPLEMENT;
        return c;
    }

    Valuable & Exponentiation::sq()
    {
        eexp() *= 2;
        optimized = {};
        optimize();
        return *this;
    }

    const Valuable::vars_cont_t& Exponentiation::getCommonVars() const
    {
        return v;
    }
    
    Valuable Exponentiation::operator()(const Variable& v, const Valuable& augmentation) const
    {
        if (!getExponentiation().FindVa() && getExponentiation()!=0 && augmentation==0) {
            return getBase()(v,augmentation);
        } else if (getExponentiation().IsSimpleFraction()) {
            auto f = Fraction::cast(getExponentiation());
            return (getBase()^f->getNumerator())(v,augmentation^f->getDenominator());
        } else {
            IMPLEMENT
        }
    }
}}
