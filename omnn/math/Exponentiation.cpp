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
#include <limits>

#include <boost/numeric/conversion/cast.hpp>

namespace omnn::math {

    max_exp_t Exponentiation::getMaxVaExp(const Valuable& b, const Valuable& e)
    {
        if (e.IsInt()) {
            return b.getMaxVaExp() * e.ca();
        } else if (e.FindVa()) {
            auto i = b.getMaxVaExp();
            if (i) {
                auto _ = e;
                _.eval(e.GetVaExps());
                OptimizeOn(), _.optimize();
                if (_.IsInt())
                    i *= static_cast<a_int>(_);
                else {
                    i *= static_cast<double>(_);
                }
            }
            return i;
        } else {
            auto maxVaExp = e * b.getMaxVaExp();
            if (!maxVaExp.IsInt()) {
                OptimizeOn o;
                maxVaExp.optimize();
            }
			if (maxVaExp.IsInt()) {
                return maxVaExp.ca();
            } else if (maxVaExp.IsSimpleFraction()) {
                auto& f = maxVaExp.as<Fraction>();
                return {f.getNumerator().ca(), f.getDenominator().ca()};
            } else {
                LOG_AND_IMPLEMENT(maxVaExp << " uncovered case for Exponentiation::getMaxVaExp")
            }
        }
        IMPLEMENT
    }

    max_exp_t Exponentiation::getMaxVaExp() const
    {
        return getMaxVaExp(getBase(), getExponentiation());
    }
    
    Valuable Exponentiation::varless() const
    {
        if (FindVa())
            return 1;
        else
        return base::varless();
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
            InitVars();
            return;
        }

        MarkAsOptimized();
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
            auto& s = eexp().as<Sum>();
            auto sz = s.size();
            auto v = 1_v;
            for(auto it = s.begin(), e = s.end();
                it != e; )
            {
                if (it->IsInt()) {
                    v *= ebase()^*it;
                    s.Delete(it);
                }
                else
                    ++it;
            }
            if (sz != s.size()) {
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
            ebase() = ebase().as<Fraction>().Reciprocal();
        }

		// e^(i*pi) = -1
                // it is a fundamental equation that gives us a hint on cross-dimmensional relations
                // because i and -1 are 1 of different signs/dimmensions
        if (ebase().Is_e()) {
            if (eexp().IsProduct()) {
                auto& p = eexp().as<Product>();
                if (p.Has(constants::pi) &&
                    p.Has(constants::i)) { // TODO : sequence does matter :
																 // e^(i*pi) =?= e^(pi*i)
																 // https://en.wikipedia.org/wiki/Commutative_property#Division,_subtraction,_and_exponentiation 
																 // what about Commutativity on irrationals product?
																 // lets assume yes for this particular expression, but there are some doubts, need a prove

						// here is implementation for case of no commutativity:
                                    // auto e = p.end();
                                    // auto it = std::find(p.begin(), e, constant::i);
                                    // auto has_i = it != e;
                                    // if (has_i) {
                                    //     it = std::find(++it, e, constant::pi);
                                    //     auto has_pi_next_to_i = it != e; // maybe sequence does matter
                                    // }

                    p /= constant::i;
                    p /= constant::pi;
                    Become(Exponentiation{-1, p});
                    return;
                }
            }
        }
        // todo : check
        if (ebase().IsSimple()) {
            if (eexp().IsProduct()) {
                auto& p = eexp().as<Product>();
                auto it = p.GetFirstOccurence<Integer>();
                auto in = ebase() ^ *it;  // IsExponentiationSimplifiable
                if (in.IsInt()) {
                    ebase() = in;
                    p.Delete(it);
                    if (p.size() == 1) {
                        eexp().optimize();
                    }
                }
            }
            if (ebase()==1) {
                if (eexp().IsInt()) {
                    Become(std::move(ebase()));
                    return;
                } else if (eexp().IsSimpleFraction() && eexp() > 0_v) {
                    auto& f = eexp().as<Fraction>();
                    auto& n = f.getNumerator();
                    if (n.IsEven() == YesNoMaybe::Yes) {
                        auto&& toOptimize = std::move(eexp());
                        toOptimize.optimize();
                        setExponentiation(std::move(toOptimize));
                        optimize();
                        return;
                    }
                    auto& dn = f.getDenominator();
                    if (dn.bit(0)) {
                        Become(std::move(ebase()));
                        return;
                    } else if (n != 1_v) {
                        hash ^= f.Hash();
                        f.update1(1_v);
                        hash ^= f.Hash();
					}
                } else if (!!eexp().IsMultival()) {
                } else if (!(eexp().IsInfinity() || eexp().IsMInfinity())) {
                    Become(std::move(ebase()));
                    return;
                } else
                    IMPLEMENT;
            } else if (ebase() == -1 && eexp().IsInt() && eexp() > 0 && eexp() != 1) {
                    eexp() = eexp().bit(0);
            } else if (eexp()==-1) {
                Become(Fraction{1,ebase()});
                return;
            } else if (eexp().IsInfinity()) {
                IMPLEMENT
            } else if (eexp().IsFraction()) {
                auto& f = eexp().as<Fraction>();
                auto& n = f.getNumerator();
                if (n != 1) {
                    // TODO: auto is = ebase().IsExponentiationSimplifiable(n);
                    auto newBase = ebase() ^ n;
                    if(!newBase.IsExponentiation()){
                        Become(newBase ^ (1_v / f.getDenominator()));
                        return;
                    }
                }
            }
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
        else if (ebase().IsVa() && eexp().IsSimple())
        {
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
                        auto& b = ebase();

                        if (b.IsVa()
							|| b.IsModulo()
							)
						{
                            break;
                        }
                        if (eexp() != constants::zero) {
                            if (eexp() > constants::one) {
                                Valuable x = b;
                                Valuable n = eexp();
                                if (n < constants::zero)
                                {
                                    x = constants::one / x;
                                    n = -n;
                                }
                                if (n == constants::zero)
                                {
                                    Become(1_v);
                                    return;
                                }
                                auto y = constants::one;
                                while (n > constants::one)
                                {
                                    bool isInt = n.IsInt();
                                    if (!isInt)
                                        IMPLEMENT
                                    if (isInt && n.bit() == constants::zero)
                                    {
                                        x.sq();
                                        n /= constants::two;
                                    }
                                    else
                                    {
                                        y *= x;
                                        x.sq();
                                        --n;
                                        n /= constants::two;
                                    }
                                }
                                x *= y;
                                Become(std::move(x));
                            } else if (eexp() != constants::minus_1) {
                                // negative
                                Become(constants::one / (ebase() ^ (-eexp())));
                            }
                        }
                        else { // zero
                            if (ebase() == constants::zero)
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
                case View::Equation: {
                    if(eexp().IsSimple())
                        Become(std::move(ebase()));
                    break;
                }
                default:
                	LOG_AND_IMPLEMENT(*this << " mode is " << static_cast<int>(view));
            }
        }

        if(IsExponentiation() && ebase().IsExponentiation())
        {
            auto& e = ebase().as<Exponentiation>();
            auto& eeexp = e.getExponentiation();
            if ((eeexp.FindVa() == nullptr) == (eexp().FindVa() == nullptr)) {
                eexp() *= eeexp;
                // todo : copy if it shared
                ebase() = std::move(const_cast<Valuable&>((e.getBase())));
            }
        }

        if (IsExponentiation()) {
            hash = ebase().Hash() ^ eexp().Hash();
            MarkAsOptimized();
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
                    auto& e = it.as<Exponentiation>();
                    if (ebase() == e.getBase()) {
                        return &e;
                    }
                }
            }
            return {};
        };
        auto& b = ebase();
        if (v.IsExponentiation()
            && b == (e = &v.as<Exponentiation>())->getBase()
            && (eexp().IsInt() || eexp().IsSimpleFraction()) && eexp() > 0
            && (e->eexp().IsInt() || e->eexp().IsSimpleFraction()) && e->eexp() > 0
            )
        {
            updateExponentiation(eexp() + e->getExponentiation());
        }
        else if(v.IsFraction()
                && (f = &v.as<Fraction>())->getDenominator() == ebase())
        {
            --eexp();
            optimized={};
            optimize();
            return *this *= f->getNumerator();
        }
        else if(v.IsFraction()
                && f->getDenominator().IsProduct()
                && (fdn = &f->getDenominator().as<Product>())->Has(ebase()))
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
        else if(b == v && v.FindVa())
        {
            updateExponentiation(eexp()+1);
        }
        else if(-b == v && v.FindVa())
        {
            updateExponentiation(eexp()+1);
            return Become(Product{-1, *this});
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
        } else if (v == constants::one) {
            is = true;
        } else if (!FindVa() && IsMultival() == YesNoMaybe::Yes && !v.FindVa()) {
            solutions_t values;
            auto distinct = Distinct();
            for (auto& value : distinct) {
                auto& extract = distinct.extract(value).value();
                if (extract.MultiplyIfSimplifiable(v)) {
                    is = true;
				} else {
                    extract *= v;
				}
                values.emplace(std::move(extract));
            }
            Become(Valuable(std::move(values)));
        } else if (v.IsExponentiation()) {
            auto& vexpo = v.as<Exponentiation>();
            is = vexpo.getBase() == getBase();
            if (is) {
                eexp() += vexpo.eexp();
                optimized = {};
                optimize();
            } // TODO : else if ? (base^2 == v.base)
            else {
                is = vexpo.getExponentiation() == getExponentiation();
                if (is) {
                    auto wasBaseHash = ebase().Hash();
                    is = ebase().MultiplyIfSimplifiable(vexpo.getBase());
                    if (is) {
                        Valuable::hash ^= wasBaseHash ^ ebase().Hash();
                        optimized = {};
                        optimize();
                    }
                }
            }
        } else if (v.IsMultival() == YesNoMaybe::Yes) {
            LOG_AND_IMPLEMENT(str() << " Exponentiation::MultiplyIfSimplifiable " << v);
        } else if (v.IsInt()) {
            IMPLEMENT
        } else {
//            std::cout << str() << " * " << v.str() << std::endl;
        }
        return is;
    }

    std::pair<bool,Valuable> Exponentiation::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is, expSumSimplifiable = {};
        is.first = v == getBase()
            && (expSumSimplifiable = eexp().IsSummationSimplifiable(constants::one)).first;
        if (is.first) {
            is.second = getBase() ^ expSumSimplifiable.second;
        } else if (v.IsExponentiation()) {
            auto& vexpo = v.as<Exponentiation>();
            is.first = vexpo.getBase() == getBase();
            if (is.first) {
                is.second = ebase() ^ (eexp() + vexpo.eexp());
            } // TODO : else if ? (base^2 == v.base)
        } else if (v.IsSimple()) {
            auto& ee = getExponentiation();
            //is = v.IsExponentiationSimplifiable(ee);  // TODO: Implement IsExponentiationSimplifiable
            // FIXME: Until IsExponentiationSimplifiable ready:
            //if (ee.IsSimpleFraction()) {
            //    is.second = v ^ ee.as<Fraction>().Reciprocal(); // v.IsExponentiationSimplifiable(ee)
            //    is.first = is.second.MultiplyIfSimplifiable(getBase());
            //    if(is.first){
            //        auto copy = *this;
            //        copy.updateBase(std::move(is.second));
            //        is.second = copy;
            //    }
            //} else 
			if (ee == constants::minus_1)
            {
                is.second = v ^ constants::minus_1; // v.IsExponentiationSimplifiable(ee)
                is.first = is.second.MultiplyIfSimplifiable(getBase());
                if(is.first){
                    auto copy = *this;
                    copy.updateBase(std::move(is.second));
                    is.second = copy;
                }
            }
            else if (ee.IsInt() && (ee > constants::zero)) // TODO: ee < 0 too
            {
//                is.second = v ^ (ee ^ constants::minus_1); // v.IsExponentiationSimplifiable(ee)
//                is.first = is.second.MultiplyIfSimplifiable(getBase());
//                if(is.first){
//                    auto copy = *this;
//                    copy.updateBase(std::move(is.second));
//                    is.second = copy;
//                }
            }
//            if (getBase().IsVa()) {
//            } else if (getExponentiation().IsSimpleFraction()) {
//                auto
//                is.first = IsMultiplicationSimplifiable()
//            } else if (getExponentiation().IsSimple()) {
//                is = base::IsMultiplicationSimplifiable(v);
//            } else {
//                IMPLEMENT
//            }
        } else if (v.IsVa()) {
            // covered by (v==base()) case
        } else if (v.IsProduct()) {
            is=v.IsMultiplicationSimplifiable(*this);
        } else if (v.IsSum()) {
            auto& sum=v.as<Sum>();
            is.first=sum==getBase()||-sum==getBase();
            if(is.first){
                is.second=*this*sum;
            }else{
                is=sum.IsMultiplicationSimplifiable(*this);
            }
        } else {
#ifndef NDEBUG
            std::cout << "IsMultiplication simplifiable?: " << str() << " * " << v.str() << std::endl;
#endif
        }
        return is;
    }

    bool Exponentiation::SumIfSimplifiable(const Valuable& v)
    {
        auto is = !v.IsSimple() && !v.IsFraction() && !v.IsExponentiation();
        if(is){
            auto sumIfSimplifiable = v.IsSummationSimplifiable(*this);
            is = sumIfSimplifiable.first;
            if (is)
                Become(std::move(sumIfSimplifiable.second));
        }
        return is;
    }

    std::pair<bool,Valuable> Exponentiation::IsSummationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is;
        is.first = operator==(v);
        if (is.first) {
            is.second = *this * 2;
        } else if ((is.first = operator==(-v))) {
                is.second = 0;
        } else if (v.IsSimple()
                || v.IsExponentiation()
                || v.IsVa()
                || v.IsFraction())
        {
        } else {
            is = v.IsSummationSimplifiable(*this);
        }
        return is;
    }

    Valuable& Exponentiation::operator /=(const Valuable& v)
    {
        auto isMultival = IsMultival()==YesNoMaybe::Yes;
        auto vIsMultival = v.IsMultival()==YesNoMaybe::Yes;
        if(isMultival && vIsMultival) {
            OptimizeOn o; // solutions sets may have equivalent values if optimizations are off
            solutions_t vals, thisValues;
            Values([&](auto& thisVal){
                thisValues.insert(thisVal);
                return true;
            });
            
            v.Values([&](auto& vVal) {
                for (auto& tv : thisValues) {
                    vals.insert(tv / vVal);
                }
                return true;
            });
            
            return Become(Valuable(std::move(vals)));
        }
        else if (v.IsExponentiation())
        {
            auto& e = v.as<Exponentiation>();
            if(ebase() == e.ebase() && (ebase().IsVa() || !ebase().IsMultival()))
            {
                eexp() -= e.eexp();
            }
            else
            {
                Become(Fraction(*this, v));
                return *this;
            }
        }
        else if(v.IsFraction())
        {
            *this *= v.as<Fraction>().Reciprocal();
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
        auto eq = v.IsExponentiation() && Hash()==v.Hash();
        if(eq){
            auto& e = v.as<Exponentiation>();
            eq = _1.Hash() == e._1.Hash()
                && _2.Hash() == e._2.Hash()
                && _1 == e._1
                && _2 == e._2;
        } else if (v.IsFraction()) {
            eq = eexp().IsInt()
                 && eexp() < 0
                 && ebase() == (v.as<Fraction>().getDenominator() ^ (-eexp()));
        } else if (v.IsProduct()) {
            eq = v.as<Product>().operator==(*this);
        }
        return eq;
    }
    
    Exponentiation::operator double() const
    {
        return std::pow(static_cast<double>(ebase()), static_cast<double>(eexp()));
    }

    Valuable& Exponentiation::d(const Variable& x)
    {
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
    
    Valuable& Exponentiation::integral(const Variable& x, const Variable& C)
    {
        if (ebase()==x) {
            auto& ee = eexp();
            if ((ee.IsInt() || ee.IsSimpleFraction())) {
                updateExponentiation(ee + 1);
                operator/=(eexp());
                operator+=(C);
            } else if (ee.IsSum()) {
                Product p;
                for (auto& m : ee.as<Sum>()) {
                    p.Add(x ^ m);
                }
                Become(std::move(p.integral(x, C)));
            }
        } else {
            IMPLEMENT
        }
        
        return *this;
    }

    bool Exponentiation::operator <(const Valuable& v) const
    {
        if (v.IsExponentiation())
        {
            auto& e = v.as<Exponentiation>();
            if (e.getBase() == getBase())
                return getExponentiation() < e.getExponentiation();
            if (e.getExponentiation() == getExponentiation())
                return getBase() < e.getBase();
        }
        
        return base::operator <(v);
    }

    std::ostream& Exponentiation::print_sign(std::ostream& out) const
    {
        return out << "^";
    }

    bool Exponentiation::IsMultiSign() const {
        return _2.IsFraction()
            && _2.as<Fraction>().getDenominator().IsEven() == YesNoMaybe::Yes;
    }
    Valuable::YesNoMaybe Exponentiation::IsMultival() const
    {
        auto is = _1.IsMultival() || _2.IsMultival();
        if (is != YesNoMaybe::Yes && _2.IsFraction())
            is = _2.as<Fraction>().getDenominator().IsEven() || is;
        return is;
    }
    
    void Exponentiation::Values(const std::function<bool(const Valuable&)>& fun) const
    {
        if (fun) {
            auto cache = optimized; // TODO: multival caching (inspect all optimized and optimization transisions) auto isCached =
            
            solutions_t vals;
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
                        static const Variable x;
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
        if (getExponentiation().IsInt()) {
            out << "(1";
            for (auto i = getExponentiation(); i-- > 0;) {
                out << '*';
                getBase().code(out);
            }
            out << ')';
        } else {
            out << "pow(";
            getBase().code(out) << ',';
            getExponentiation().code(out) << ')';
        }       
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
            is = !(v.IsComesBefore(*this) || operator==(v));
        }
        else if(v.IsInt())
            is = true;
//        else if(v.IsFraction())
//        {is=}
        else if(v.IsVa())
            is = !!FindVa();
        else if(v.IsSum())
            is = IsComesBefore(*v.as<Sum>().begin());
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

    Valuable Exponentiation::Sqrt() const {
        Valuable copy = *this;
        copy.sqrt();
        return std::move(copy);
    }

    Valuable& Exponentiation::sqrt() {
		hash ^= _2.Hash();
        _2 /= 2;
        hash ^= _2.Hash();
        optimized = {};
        optimize();
        return *this;
    }

    const Valuable::vars_cont_t& Exponentiation::getCommonVars() const
    {
#ifndef NDEBUG
        auto& b = ebase();
        if (b.IsVa()) { // TODO: FindVa too
            auto& va = b.as<Variable>();
            auto it = v.find(va);
            if (it != v.end() && !it->second.Same(eexp())) {
                for (auto& p : v) {
                    std::cout << p.first << " -> " << p.second << std::endl;
                }

                //<< (v.begin()->first == b)
                LOG_AND_IMPLEMENT(*this << " Exponentiation::getCommonVars not ready, no " << b << " in v "
				);
            }
        }
#endif
        return v;
    }
    
    Valuable::vars_cont_t Exponentiation::GetVaExps() const
    {
        auto vaExps = getBase().GetVaExps();
        auto& e = getExponentiation();
        for (auto& ve : vaExps) {
            ve.second *= e;
        }
        return vaExps;
    }
    
    Valuable Exponentiation::InCommonWith(const Valuable& v) const
    {
        auto c = 1_v;
        if (v.IsProduct()) {
            for(auto& m: v.as<Product>()){
                c = InCommonWith(m);
                if (c != 1_v) {
                    break;
                }
            }
        } else if (v.IsExponentiation()) {
            auto& e = v.as<Exponentiation>();
            if (e.getBase() == getBase()) {
                if (e.getExponentiation() == getExponentiation()) {
                    c = e;
                } else if (getExponentiation().IsSimple() && e.getExponentiation().IsSimple()) {
                    if (getExponentiation() > 0 || e.getExponentiation() > 0) {
                        if (e.getExponentiation() >= getExponentiation()) {
                            c = *this;
                        } else
                            c = e;
                    } else if (getExponentiation() < 0 || e.getExponentiation() < 0) {
                        if (e.getExponentiation() >= getExponentiation()) {
                            c = e;
                        } else
                            c = *this;
                    } else {
                        IMPLEMENT
                    }
                } else if (getExponentiation().IsSimpleFraction() && e.getExponentiation().IsSimpleFraction()) {
                    if (getExponentiation()<0 == e.getExponentiation()<0) {
                        c = getBase() ^ getExponentiation().InCommonWith(e.getExponentiation());
                    }
                } else if (getExponentiation().IsSum()) {
                    auto sz = getExponentiation().as<Sum>().size();
                    auto diff = getExponentiation() - e.getExponentiation();
                    if (!diff.IsSum() || diff.as<Sum>().size() < sz)
                        c = v;
                } else if (e.getExponentiation().IsSum()) {
                    c = e.InCommonWith(*this);
                } else if (e.getExponentiation().IsProduct()) {
                    c = ebase() ^ e.eexp().InCommonWith(eexp());
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
        } else if (v.IsModulo()) {
        } else if (getExponentiation().IsVa()) {
        } else if (getExponentiation().IsModulo()) {
        } else {
            IMPLEMENT
        }
        return c;
    }
    
    Valuable Exponentiation::operator()(const Variable& va) const
    {
        return operator()(va, constants::zero);
    }

    Valuable Exponentiation::operator()(const Variable& v, const Valuable& augmentation) const
    {
        if (!getExponentiation().FindVa() && getExponentiation()!=0 && augmentation==0) {
            return getBase()(v,augmentation);
        } else if (getExponentiation().IsSimpleFraction()) {
            auto& f = getExponentiation().as<Fraction>();
            return (getBase()^f.getNumerator())(v,augmentation^f.getDenominator());
        } else {
            IMPLEMENT
        }
    }

    Valuable::solutions_t Exponentiation::Distinct() const
    {
        solutions_t branches;
        if (eexp().IsSimpleFraction()){
            auto& f = eexp().as<Fraction>();
            auto& denom = f.denominator();
            if (denom.IsEven() == YesNoMaybe::Yes) {
                // TODO : de-recoursefy:
//                auto branchesSz = boost::multiprecision::msb(denom); // the largest bit
//                branches.reserve(branchesSz);
//                ...
                auto& exponentiationBase = ebase();
                if(!exponentiationBase.IsSimple()){
                    if (!exponentiationBase.FindVa() && exponentiationBase.IsMultival() == YesNoMaybe::Yes) {
                        for (auto&& branch : ebase().Distinct()) {
                            auto branchDistinct = (branch ^ eexp()).Distinct();
                            branches.insert(branchDistinct.begin(), branchDistinct.end());
                        }
                    } else if(denom == constants::two) {
                        auto branch = exponentiationBase.Sqrt() * f.numerator();
                        branches.emplace(-branch);
                        branches.emplace(std::move(branch));
                    }
                    else {
                        LOG_AND_IMPLEMENT("Distinct for " << *this);
                    }
                } else {
                    auto b = ebase() ^ f.numerator();
                    auto d = denom;
                    do {
                        b.sqrt();
                        d.shr();
                    } while(d.IsEven() == Valuable::YesNoMaybe::Yes);
                    auto _ = b ^ (1_v / d);
                    auto distinct = _.Distinct();
                    if(denom==2)
                        for (auto& branch : distinct)
                        {
                            branches.emplace(-branch);
                            branches.emplace(std::move(const_cast<decltype(distinct)::reference>(branch)));
                        }
                    else if(denom==4)
                        for (auto& branch : distinct)
                        {
                            branches.emplace(-branch * constants::i);
                            branches.emplace(branch * constants::i);
                            branches.emplace(-branch);
                            branches.emplace(std::move(const_cast<decltype(distinct)::reference>(branch)));
                        }
                    else
                        LOG_AND_IMPLEMENT("Implement support for " << denom << " dimmensions");
                }
            }
        } else {
            branches.emplace(*this);
        }
        return branches;
    }

    bool Exponentiation::IsPolynomial(const Variable& v) const {
        return IsVaExp()
            && eexp().IsInt()
            && eexp() > constants::zero;
	}

    void Exponentiation::solve(const Variable& va, solutions_t& s) const {
        if (_1 == va
			&& !_2.FindVa()
			&& _2 != constants::zero)
		{
            s.emplace(constants::zero);
        } else {
			IMPLEMENT
        }
    }

    Valuable Exponentiation::Sign() const {
        return ebase().Sign() ^ eexp();
    }

}
