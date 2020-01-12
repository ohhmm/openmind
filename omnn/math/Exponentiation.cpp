//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Exponentiation.h"

#include "e.h"
#include "i.h"
#include "Infinity.h"
#include "pi.h"
#include "Integer.h"
#include "Sum.h"
#include "Product.h"

#include <cmath>

namespace omnn{
namespace math {

    Exponentiation::Exponentiation(const Valuable& _1, const Valuable& _2)
    : base(_1,_2)
    {
        InitVars();
    }

    max_exp_t Exponentiation::getMaxVaExp(const Valuable& b, const Valuable& e)
    {
        if (e.IsInt()) {
            return b.getMaxVaExp() * e.ca();
        } else if (e.FindVa()) {
            auto i = b.getMaxVaExp();
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
            auto maxVaExp = e * b.getMaxVaExp();
            if (maxVaExp.IsInt()) {
                return maxVaExp.ca();
            } else if (maxVaExp.IsSimpleFraction()) {
                auto f = Fraction::cast(maxVaExp);
                return {f->getNumerator().ca(), f->getDenominator().ca()};
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

    max_exp_t Exponentiation::getMaxVaExp() const
    {
        return getMaxVaExp(getBase(), getExponentiation());
    }
    
	Valuable Exponentiation::operator -() const
    {
        return Product {*this, -1};
    }

    void Exponentiation::InitVars() {
        v.clear();
        if (ebase().IsVa())
            v[ebase().as<Variable>()] = eexp();
    }

    void Exponentiation::optimize()
    {
        if (optimized) {
            return;
        }

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

        // todo : check it, comment this and try System test
//        if (ebase().IsSum() && eexp().IsFraction())
//        {
//            auto f = Fraction::cast(eexp());
//            auto& d = f->getDenominator();
//            if (d == ebase().getMaxVaExp()) {
//                auto vars = ebase().Vars();
//                if (vars.size() == 1) {
//                    auto va = *vars.begin();
//                    auto baseToSolve = ebase();
//                    baseToSolve.SetView(View::Solving);
//                    baseToSolve.optimize();
//                    auto eq = va - baseToSolve(va);
//                    auto sq = eq ^ d;
//                    auto check = ebase()/sq;
//                    if (check.IsInt() || check.IsSimpleFraction()) {
//                        ebase() = eq;
//                        eexp() = f->getNumerator();
//                    } else {
//                        // TODO : IMPLEMENT
//                    }
//                }
//            }
//        }
//
//        if (ebase().IsSum() && eexp().IsFraction())
//        {
//            auto f = Fraction::cast(eexp());
//            auto& d = f->getDenominator();
//            auto e = ebase() ^ f->getNumerator();
//            if (d == e.getMaxVaExp()) {
//                auto vars = e.Vars();
//                if (vars.size() == 1) {
//                    auto va = *vars.begin();
//                    auto eq = va - e(va);
//                    auto sq = eq ^ d;
//                    auto check = e / sq;
//                    if (check.IsInt() || check.IsSimpleFraction()) {
//                        Become(std::move(eq));
//                        return;
//                    } else {
//                        // TODO : IMPLEMENT
//                    }
//                }
//            }
//        }
        
        if (ebase().IsFraction() && eexp().IsMultival()==YesNoMaybe::No) {
            auto& f = ebase().as<Fraction>();
            auto _ = (f.getNumerator() ^ eexp()) / (f.getDenominator() ^ eexp());
            if (_.IsExponentiation()) {
                auto& e = _.as<Exponentiation>();
                if (!(e.ebase()==ebase() && eexp()==e.eexp())) {
                    IMPLEMENT
                }
            } else {
                Become(std::move(_));
                return;
            }
        }

        if (ebase().IsFraction() && eexp().IsInt() && eexp() < 0_v) {
            eexp() = -eexp();
            ebase() = Fraction::cast(ebase())->Reciprocal();
        }

        if (ebase().Is_e()) {
            if (eexp().IsProduct()) {
                auto p = Product::cast(eexp());
                if (p->Has(constant::pi) && p->Has(constant::i)) { // TODO : sequence does metter
//                    ebase() = -1;
//                    eexp() /= constant::i;
//                    eexp() /= constant::pi;
                }
            }
        }

        if (ebase().IsInt()) {
            if (ebase()==1) {
                if (eexp().IsInt()) {
                    Become(std::move(ebase()));
                    return;
                } else if (eexp().IsSimpleFraction()) {
                    if (Fraction::cast(eexp())->getDenominator().bit(0)) {
                        Become(std::move(ebase()));
                        return;
                    }
                } else
                    IMPLEMENT;
            } else if (ebase()==-1 && eexp().IsInt() && eexp() > 0 && eexp()!=1) {
                eexp() = eexp().bit(0);
            } else if (eexp()==-1) {
                Become(Fraction{1,ebase()});
                return;
            } else if (eexp().IsInfinity())
                IMPLEMENT
        }

        bool ebz = ebase() == 0_v;
        bool exz = eexp() == 0_v;
        if(exz)
        {
            if (ebase().IsInfinity() || ebase().IsMInfinity()) {
                IMPLEMENT
            }
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
        else if (ebase().IsInfinity())
        {
            if (eexp() > 0) {
                Become(std::move(ebase()));
            } else
                IMPLEMENT
        }
        else if (ebase().IsMInfinity())
        {
            if (eexp() > 0) {
                if ((eexp() % 2) > 0) // TODO : test with non-ints
                    Become(std::move(ebase()));
                else
                    Become(Infinity());
            } else
                IMPLEMENT
        }
        else
        {
            switch(view)
            {
                case View::Solving:

                case View::None:
                case View::Calc:
                {
                    if (eexp().IsInt() && eexp()>0) {
                        auto b = 1_v;
                        for (; eexp()--;) {
                            b *= ebase();
                        }
                        Become(std::move(b));
                        return;
                    }
                    if (ebase().IsInt() && eexp().IsInt()) {
                        Become(ebase() ^ eexp());
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
                                    if (isInt && n.bit(0) == 0_v)
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
            InitVars();
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
            && ebase() == (e = &v.as<Exponentiation>())->getBase()
            && (eexp().IsInt() || eexp().IsSimpleFraction()) && eexp() > 0
            && (e->eexp().IsInt() || e->eexp().IsSimpleFraction()) && e->eexp() > 0
            )
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
        else if(v.IsInt())
        {
            if(v==1)
                return *this;
            else if(eexp()==-1 && ebase().IsInt())
                return Become(v/ebase());
            else
                return Become(Product{v, *this});
        }
        else
            return Become(Product{v, *this});

        optimize();
        return *this;
    }

    bool Exponentiation::MultiplyIfSimplifiable(const Valuable& v)
    {
        auto is = v == getBase();
        if (is) {
            ++eexp();
            optimized = {};
            optimize();
        } else if (v.IsExponentiation()) {
            auto& vexpo = v.as<Exponentiation>();
            is = vexpo.getBase() == getBase();
            if (is) {
                eexp() += vexpo.eexp();
                optimized = {};
                optimize();
            } // TODO : else if ? (base^2 == v.base)
        } else if (v.IsInt()) {
            IMPLEMENT
        } else {
//            std::cout << str() << " * " << v.str() << std::endl;
        }
        return is;
    }

    std::pair<bool,Valuable> Exponentiation::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is;
        is.first = v == getBase();
        if (is.first) {
            is.second = getBase() ^ (getExponentiation()+1);
        } else if (v.IsExponentiation()) {
            auto& vexpo = v.as<Exponentiation>();
            is.first = vexpo.getBase() == getBase();
            if (is.first) {
                is.second = ebase() ^ (eexp() + vexpo.eexp());
            } // TODO : else if ? (base^2 == v.base)
        } else if (v.IsInt()) {
//            if (getBase().IsVa()) {
//            } else if (getExponentiation().IsSimpleFraction()) {
//                auto
//                is.first = IsMultiplicationSimplifiable()
//            } else if (getExponentiation().IsSimple()) {
//                is = base::IsMultiplicationSimplifiable(v);
//            } else {
//                IMPLEMENT
//            }
        } else {
            std::cout << str() << " * " << v.str() << std::endl;
        }
        return is;
    }

    Valuable& Exponentiation::operator /=(const Valuable& v)
    {
        auto isMultival = IsMultival()==YesNoMaybe::Yes;
        auto vIsMultival = v.IsMultival()==YesNoMaybe::Yes;
        if(isMultival && vIsMultival) {
            solutions_t vals, thisValues;
            Values([&](auto& thisVal){
                thisValues.insert(thisVal);
                return true;
            });
            
            v.Values([&](auto&vVal){
                for(auto& tv:thisValues)
                    vals.insert(tv/vVal);
                return true;
            });
            
            return Become(Valuable(vals));
        }
        else if (v.IsExponentiation())
        {
            auto e = cast(v);
            if(ebase() == e->getBase() && !ebase().IsMultival())
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
    
    bool Exponentiation::operator ==(const Valuable& v) const
    {
        const Exponentiation* ch;
        return (v.IsExponentiation()
                && Hash()==v.Hash()
                && (ch = cast(v))
                && _1.Hash() == ch->_1.Hash()
                && _2.Hash() == ch->_2.Hash()
                && _1 == ch->_1
                && _2 == ch->_2
                ) ||
                (v.IsFraction()
                 && eexp().IsInt()
                 && eexp() < 0
                 && ebase() == (Fraction::cast(v)->getDenominator() ^ (-eexp()))
                );
    }
    
    Exponentiation::operator double() const
    {
        return std::pow(static_cast<double>(ebase()), static_cast<double>(eexp()));
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
    
    Valuable& Exponentiation::i(const Variable& x, const Variable& C)
    {
        if ((eexp().IsInt() || eexp().IsSimpleFraction()) && ebase()==x) {
            ++eexp();
            operator/=(eexp());
            operator+=(C);
        } else {
            IMPLEMENT
        }
        
        optimize();
        return *this;
    }

    Valuable Exponentiation::I(const Variable& x, const Variable& C) const
    {
        if ((eexp().IsInt() || eexp().IsSimpleFraction()) && ebase()==x) {
            auto einc = eexp()+1;
            return (ebase() ^ einc) / einc + C;
        } else {
            IMPLEMENT
        }
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

    Valuable::YesNoMaybe Exponentiation::IsMultival() const
    {
        auto is = _1.IsMultival() || _2.IsMultival();
        if (is != YesNoMaybe::Yes)
            if(_2.IsFraction())
                is = _2.as<Fraction>().getDenominator().IsEven() || is;
        return is;
    }
    
    void Exponentiation::Values(const std::function<bool(const Valuable&)>& fun) const
    {
        if (fun) {
            auto cache = optimized; // TODO: multival caching (inspect all optimized and optimization transisions) auto isCached =
            
            std::set<Valuable> vals;
            {
            std::deque<Valuable> d1;
            _1.Values([&](auto& v){
                d1.push_back(v);
                return true;
            });
            
            _2.Values([&](auto& v){
                auto vIsFrac = v.IsFraction();
                const Fraction* f;
                if(vIsFrac)
                    f = &v.template as<Fraction>();
                auto vMakesMultival = vIsFrac && f->getDenominator().IsEven()==YesNoMaybe::Yes;
                
                for(auto& item1:d1){
                    if(vMakesMultival){
                        Variable x;
                        auto& dn = f->getDenominator();
                        auto solutions = (x ^ dn).Equals(*this ^ dn).Solutions(x);
                        for(auto&& s:solutions)
                            vals.insert(s);
                    } else {
                        auto value=item1^v;
                        if(value.IsMultival()==YesNoMaybe::No)
                            vals.insert(value);
                        else {
                            IMPLEMENT
                        }
                    }
                }
                return true;
            });
            }
            
            for(auto& v:vals)
                fun(v);
        }
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
            auto& e = v.as<Exponentiation>();
            bool baseIsVa = getBase().IsVa();
            bool vbaseIsVa = e.getBase().IsVa();
            if (baseIsVa && vbaseIsVa)
                is = getExponentiation() == e.getExponentiation() ? getBase().IsComesBefore(e.getBase()) : getExponentiation() > e.getExponentiation();
            else if(baseIsVa)
                is = false;
            else if(vbaseIsVa)
                is = true;
            else if(getBase() == e.ebase())
                is = getExponentiation().IsComesBefore(e.getExponentiation());
            else if(getExponentiation() == e.getExponentiation())
                is = getBase().IsComesBefore(e.getBase());
            else
            {
                auto c = Complexity();
                auto ec = e.Complexity();
                if (c != ec)
                    is = c > ec;
                else {
                    is = getBase().IsComesBefore(e.getBase()) || 
                        (!e.ebase().IsComesBefore(ebase()) && getExponentiation().IsComesBefore(e.getExponentiation())); //  || str().length() > e->str().length();
    //                auto expComesBefore = eexp().IsComesBefore(e->eexp());
    //                auto ebase()ComesBefore = ebase().IsComesBefore(e->ebase());
    //                is = expComesBefore==ebase()ComesBefore || str().length() > e->str().length();
                }
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
        else if(v.IsSum())
            is = IsComesBefore(*Sum::cast(v)->begin());
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
    
    Valuable Exponentiation::InCommonWith(const Valuable& v) const
    {
        auto c = 1_v;
        if (v.IsProduct()) {
            for(auto& m: Product::cast(v)->GetConstCont()){
                c = InCommonWith(m);
                if (c != 1_v) {
                    break;
                }
            }
        } else if (v.IsExponentiation()) {
            auto e = Exponentiation::cast(v);
            if (e->getBase() == getBase()) {
                if (e->getExponentiation() == getExponentiation()) {
                    c = *e;
                } else if (getExponentiation().IsInt() && e->getExponentiation().IsInt()) {
                    if (getExponentiation() > 0 || e->getExponentiation() > 0) {
                        if (e->getExponentiation() >= getExponentiation()) {
                            c = *this;
                        } else
                            c = *e;
                    } else if (getExponentiation() < 0 || e->getExponentiation() < 0) {
                        if (e->getExponentiation() >= getExponentiation()) {
                            c = *e;
                        } else
                            c = *this;
                    } else {
                        IMPLEMENT
                    }
                } else if (getExponentiation().IsSimpleFraction() && e->getExponentiation().IsSimpleFraction()) {
                    if (getExponentiation()<0 == e->getExponentiation()<0) {
                        IMPLEMENT
                    }
                } else if (getExponentiation().IsSum()) {
                    auto sz = getExponentiation().as<Sum>().size();
                    auto diff = getExponentiation() - e->getExponentiation();
                    if (!diff.IsSum() || diff.as<Sum>().size() < sz)
                        c = v;
                } else if (e->getExponentiation().IsSum()) {
                    c = e->InCommonWith(*this);
                } else {
                    IMPLEMENT
                }
            }
        } else if (getExponentiation().IsInt()) {
            if(getExponentiation() > 0)
                c = getBase().InCommonWith(v);
        } else if (getExponentiation().IsFraction()) {
        } else if (v.IsVa()) {
            c = v.InCommonWith(*this);
        } else if (v.IsInt() || v.IsSimpleFraction()) {
        } else if (getExponentiation().IsVa()) {
        } else {
            IMPLEMENT
        }
        return c;
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
