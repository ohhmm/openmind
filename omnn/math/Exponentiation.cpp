//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Exponentiation.h"
#include "CompilerMacros.h"
#include "DuoValDescendant.h"
#include "Euler.h"
#include "i.h"
#include "Infinity.h"
#include "pi.h"
#include "Integer.h"
#include "PrincipalSurd.h"
#include "Product.h"
#include "Sum.h"
#include "Constant.h"
#include "Valuable.h"
#include "Variable.h"
#include "Fraction.h"
#include "constants.h"

#include <cmath>
#include <limits>
#include <boost/numeric/conversion/cast.hpp>

namespace omnn::math {

void Exponentiation::InitVars() {
    this->v.clear();
    if (auto va = this->_1.FindVa()) {
        this->v.insert(*va);
    }
    if (auto va = this->_2.FindVa()) {
        this->v.insert(*va);
    }
}

max_exp_t Exponentiation::getMaxVaExp(const Valuable& b, const Valuable& e) {
    if(b.FindVa() || e.FindVa()) {
        return std::numeric_limits<max_exp_t>::max();
    }
    return {};
}

std::ostream& Exponentiation::print_sign(std::ostream& out) const {
    return out << '^';
}

std::ostream& Exponentiation::code(std::ostream& out) const {
    out << "std::pow(";
    PrintCodeVal(out, this->_1);
    out << ", ";
    PrintCodeVal(out, this->_2);
    return out << ')';
}

Valuable Exponentiation::varless() const {
    if (this->FindVa())
        return omnn::math::constants::one;
    else
        return base::varless();
}

void Exponentiation::optimize() {
    if (this->optimized)
        return;

    if (this->IsEquation()) {
        this->Become(std::move(this->_1));
        return;
    }

    this->_1.optimize();
    this->_2.optimize();

    if (this->ebase().IsExponentiation() || this->ebase().IsProduct()) {
        this->ebase() ^= this->eexp();
        this->Become(std::move(this->ebase()));
        return;
    }

    if (this->eexp().IsSum()) {
        auto& s = this->eexp().as<Sum>();
        auto sz = s.size();
        auto v = omnn::math::constants::one;
        for(auto it = s.begin(), e = s.end();
            it != e; ) {
            if (it->IsInt()) {
                v *= this->ebase()^*it;
                s.Delete(it);
            }
            else
                ++it;
        }
        if (sz != s.size()) {
            this->Become(*this * v);
            return;
        }
    }

    if (this->ebase().IsSum()) {
        if (this->eexp() == omnn::math::constants::minus_1) {
            auto& s = this->ebase().as<Sum>();
            auto lcm = s.LCMofMemberFractionDenominators();
            if (lcm != omnn::math::constants::one) {
                this->ebase() *= lcm;
                this->Become(*this * lcm);
                return;
            }
        }
    }

    if (this->ebase().IsFraction() && this->eexp().IsMultival()==YesNoMaybe::No) {
        auto& f = this->ebase().as<Fraction>();
        auto _ = (f.getNumerator() ^ this->eexp()) / (f.getDenominator() ^ this->eexp());
        if (_.IsExponentiation()) {
            auto& e = _.as<Exponentiation>();
            if (!(e.ebase()==this->ebase() && this->eexp()==e.eexp())) {
                auto simplified = (e.ebase() ^ e.eexp()).optimize();
                if (simplified != *this) {
                    this->Become(std::move(simplified));
                }
            }
        } else {
            this->Become(std::move(_));
            return;
        }
    }

    if (this->ebase().IsFraction() && this->eexp().IsInt() && this->eexp() < omnn::math::constants::zero) {
        this->eexp() = -this->eexp();
        this->ebase() = this->ebase().as<Fraction>().Reciprocal();
    }

    if (this->ebase().Is_e()) {
        if (this->eexp().IsProduct()) {
            auto& p = this->eexp().as<Product>();
            if (p.Has(omnn::math::constants::pi) &&
                p.Has(omnn::math::constants::i)) {
                p /= omnn::math::constants::i;
                p /= omnn::math::constants::pi;
                this->Become(Exponentiation{-1, p});
                return;
            }
        }
    }

    if (this->ebase().IsSimple()) {
        if (this->eexp().IsProduct()) {
            auto& p = this->eexp().as<Product>();
            auto it = p.GetFirstOccurence<Integer>();
            auto in = this->ebase() ^ *it;
            if (in.IsInt()) {
                this->ebase() = in;
                p.Delete(it);
                if (p.size() == 1) {
                    this->eexp().optimize();
                }
            }
        }

        if (this->ebase() == omnn::math::constants::one) {
            if (this->eexp().IsSimple()) {
                if (this->eexp() < omnn::math::constants::zero) {
                    this->setExponentiation(-this->eexp());
                }
                else if (this->eexp().IsZero()) {
                    this->Become(std::move(this->ebase()));
                    return;
                }
            }

            if (this->eexp().IsInt()) {
                this->Become(std::move(this->ebase()));
                return;
            } else if (this->eexp().IsSimpleFraction() && this->eexp() > omnn::math::constants::zero) {
                auto& f = this->eexp().as<Fraction>();
                auto& n = f.getNumerator();
                if (n.IsEven() == YesNoMaybe::Yes) {
                    auto&& toOptimize = std::move(this->eexp());
                    toOptimize.optimize();
                    this->setExponentiation(std::move(toOptimize));
                    this->optimize();
                    return;
                }
                auto& dn = f.getDenominator();
                if (dn.bit()) {
                    this->Become(std::move(this->ebase()));
                    return;
                } else if (n != omnn::math::constants::one) {
                    this->hash ^= f.Hash();
                    f.update1(1_v);
                    this->hash ^= f.Hash();
                }
            } else if (!!this->eexp().IsMultival()) {
            } else if (!(this->eexp().IsInfinity() || this->eexp().IsMInfinity())) {
                this->Become(std::move(this->ebase()));
                return;
            } else
                IMPLEMENT;
        } else if (this->ebase() == -1 && this->eexp().IsInt() && this->eexp() > 0 && this->eexp() != 1) {
            this->eexp() = this->eexp().bit(0);
        } else if (this->eexp()==-1) {
            this->Become(Fraction{1,this->ebase()});
            return;
        } else if (this->eexp().IsInfinity()) {
            if (std::abs(static_cast<double>(this->ebase())) > 1.0) {
                this->Become(this->eexp());
            } else if (std::abs(static_cast<double>(this->ebase())) < 1.0) {
                this->Become(0_v);
            } else {
                this->Become(1_v);
            }
            throw "NaN";
        }
        return;
    }

    bool ebz = this->ebase().IsZero();
    bool exz = this->eexp().IsZero();
    if(exz) {
        if (this->ebase().IsInfinity() || this->ebase().IsMInfinity()) {
            this->Become(1_v);
            return;
        }
        if(ebz)
            throw "NaN";

        this->Become(1_v);
        return;
    }
    else if(this->eexp() == omnn::math::constants::one) {
        this->Become(std::move(this->ebase()));
        return;
    }
    else if (ebz) {
        if (exz)
            throw "NaN";
        this->Become(0_v);
        return;
    }
    else if (this->ebase().IsInfinity()) {
        if (this->eexp() > 0) {
            this->Become(std::move(this->ebase()));
        } else if (this->eexp() < 0) {
            this->Become(0_v);
        } else {
            throw "NaN";
        }
    }
    else if (this->ebase().IsMInfinity()) {
        if (this->eexp() > 0) {
            if ((this->eexp() % 2) > 0)
                this->Become(std::move(this->ebase()));
            else
                this->Become(Infinity());
        } else if (this->eexp() < 0) {
            this->Become(0_v);
        } else {
            throw "NaN";
        }
    }
    else if (this->ebase().IsVa() && this->eexp().IsSimple()) {
    }
    else {
        switch(this->view) {
            case View::Solving:
            case View::None:
            case View::Calc: {
                if (this->eexp().IsInt() && this->eexp()>0) {
                    auto b = omnn::math::constants::one;
                    for (; this->eexp()--;) {
                        b *= this->ebase();
                    }
                    this->Become(std::move(b));
                    return;
                }
                if (this->ebase().IsInt() && this->eexp().IsInt()) {
                    this->Become(this->ebase() ^ this->eexp());
                    return;
                }
                break;
            }
            case View::Flat: {
                if(this->eexp().IsInt()) {
                    auto& b = this->ebase();
                    if (b.IsVa() || b.IsModulo()) {
                        break;
                    }
                    if (this->eexp() != omnn::math::constants::zero) {
                        if (this->eexp() > omnn::math::constants::one) {
                            Valuable x = b;
                            Valuable n = this->eexp();
                            if (n < omnn::math::constants::zero) {
                                x = omnn::math::constants::one / x;
                                n = -n;
                            }
                            if (n.IsZero()) {
                                this->Become(1_v);
                                return;
                            }
                            auto y = omnn::math::constants::one;
                            while (n > omnn::math::constants::one) {
                                bool isInt = n.IsInt();
                                if (!isInt) {
                                    x = this->ebase() ^ n;
                                    n = omnn::math::constants::zero;
                                }
                                if (isInt && n.bit().IsZero()) {
                                    x.sq();
                                    n /= omnn::math::constants::two;
                                }
                                else {
                                    y *= x;
                                    x.sq();
                                    --n;
                                    n /= omnn::math::constants::two;
                                }
                            }
                            x *= y;
                            this->Become(std::move(x));
                        } else if (this->eexp() != omnn::math::constants::minus_1) {
                            this->Become(omnn::math::constants::one / (this->ebase() ^ (-this->eexp())));
                        }
                    }
                    else {
                        if (this->ebase().IsZero()) {
                            throw std::domain_error("Zero raised to zero power is undefined");
                        }
                        else {
                            this->Become(1_v);
                        }
                    }
                }
                else {
                    if (this->ebase().IsSimple() && !this->eexp().FindVa()) {
                        auto result = std::pow(static_cast<double>(this->ebase()), static_cast<double>(this->eexp()));
                        this->Become(Valuable(result));
                        return;
                    }
                }
                break;
            }
            case View::Fraction: {
                if (this->eexp().IsSimple() && this->eexp() < omnn::math::constants::zero) {
                    this->updateExponentiation(-this->eexp());
                    this->Become(Fraction{omnn::math::constants::one, std::move(this->ebase().reciprocal())});
                }
                break;
            }
            default: {
                base::optimize();
            }
        }
        if (this->IsEquation()) {
            if (this->eexp().IsSimple())
                this->Become(std::move(this->ebase()));
        }
    }

    if(this->IsExponentiation() && this->ebase().IsExponentiation()) {
        auto& e = this->ebase().as<omnn::math::Exponentiation>();
        auto& eeexp = e.getExponentiation();
        if ((eeexp.FindVa() == nullptr) == (this->eexp().FindVa() == nullptr)) {
            this->eexp() *= eeexp;
            this->ebase() = std::move(const_cast<Valuable&>((e.getBase())));
        }

            if (eexp() == omnn::math::constants::one || IsEquation()) {
                Become(std::move(ebase()));
                return;
            }
        }

        if (IsExponentiation()) {
            hash = ebase().Hash() ^ eexp().Hash();
            MarkAsOptimized();
            InitVars();
        }
    }

    Valuable& Exponentiation::operator+=(const Valuable& v) {
        return Become(Sum{*this, v});
    }

    Valuable& Exponentiation::operator*=(const Valuable& v) {
        const Exponentiation* e;
        const Fraction* f;
        const Product* fdn = {};
        const Exponentiation* fdne;
        auto isProdHasExpWithSameBase = [this](const Product* p) -> const Exponentiation* {
            for(auto& it : *p) {
                if (it.IsExponentiation()) {
                    auto& e = it.as<omnn::math::Exponentiation>();
                    if (this->ebase() == e.getBase()) {
                        return &e;
                    }
                }
            }
            return nullptr;
        };
        auto& b = ebase();
        if (v.IsExponentiation()
            && b == (e = &v.as<Exponentiation>())->getBase()
            && (eexp().IsInt() || eexp().IsSimpleFraction())
            && (e->eexp().IsInt() || e->eexp().IsSimpleFraction()))
        {
            updateExponentiation(eexp() + e->getExponentiation());
        }
        else if(v.IsFraction() && (f = &v.as<Fraction>())->getDenominator() == ebase())
        {
            --eexp();
            optimized = {};
            optimize();
            return *this *= f->getNumerator();
        }
        else if(v.IsFraction()
                && f->getDenominator().IsProduct()
                && (fdn = &f->getDenominator().as<Product>())->Has(ebase()))
        {
            --eexp();
            optimized = {};
            optimize();
            return *this *= f->getNumerator() / (*fdn / ebase());
        }
        else if(fdn
                && (fdne = isProdHasExpWithSameBase(fdn)))
        {
            eexp() -= fdne->getExponentiation();
            optimized = {};
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
        bool is = v == this->getBase();
        if (is) {
            ++this->eexp();
            optimized = {};
            optimize();
            return true;
        }

        if (v == omnn::math::constants::one) {
            return true;
        }

        if (!FindVa() && IsMultival() == YesNoMaybe::Yes && !v.FindVa()) {
            solutions_t values;
            auto distinct = Distinct();
            for (auto& value : distinct) {
                auto extracted = distinct.extract(value).value();
                if (extracted.MultiplyIfSimplifiable(v)) {
                    values.emplace(std::move(extracted));
                    is = true;
                }
            }
            if (is) {
                Become(Valuable(std::move(values)));
            }
            return is;
        }

        if (v.IsExponentiation()) {
            auto& vexpo = v.as<Exponentiation>();
            is = vexpo.getBase() == this->getBase();
            if (is) {
                this->eexp() += vexpo.eexp();
                optimized = {};
                optimize();
            } // TODO : else if ? (base^2 == v.base)
            else {
                is = vexpo.getExponentiation() == this->getExponentiation();
                if (is) {
                    auto wasBaseHash = this->ebase().Hash();
                    is = this->ebase().MultiplyIfSimplifiable(vexpo.getBase());
                    if (is) {
                        Valuable::hash ^= wasBaseHash ^ this->ebase().Hash();
                        optimized = {};
                        optimize();
                    }
                }
            }
        }

        if (v.IsMultival() == YesNoMaybe::Yes) {
            solutions_t values;
            v.Values([this, &values, &is](const Valuable& val) -> bool {
                auto result = *this;
                if (result.MultiplyIfSimplifiable(val)) {
                    values.insert(std::move(result));
                    is = true;
                }
                return true;
            });
            if (is) {
                Become(Valuable(std::move(values)));
            }
            return is;
        }

        if (v.IsInt()) {
            is = v == this->ebase();
            if (is) {
                ++this->eexp();
                optimized = {};
                optimize();
            }
        }

        return is;
    }

    std::pair<bool,Valuable> Exponentiation::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is, expSumSimplifiable = {};
        is.first = v == this->getBase()
            && (expSumSimplifiable = this->eexp().IsSummationSimplifiable(omnn::math::constants::one)).first;
        if (is.first) {
            is.second = this->getBase() ^ expSumSimplifiable.second;
        } else if (v.IsExponentiation()) {
            auto& vexpo = v.as<Exponentiation>();
            is.first = vexpo.getBase() == this->getBase();
            if (is.first) {
                is.second = this->ebase() ^ (this->eexp() + vexpo.eexp());
            } // TODO : else if ? (base^2 == v.base)
        } else if (v.IsSimple()) {
            auto& ee = this->getExponentiation();
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
			if (ee == omnn::math::constants::minus_1)
            {
                auto gcd = this->ebase().GCD(v);
                is.first = gcd != omnn::math::constants::one;
                if(is.first) {
                    is.second = (v/gcd)/(this->ebase()/gcd);
                } else {
                    is.second = v ^ omnn::math::constants::minus_1;
                    is.first = is.second.MultiplyIfSimplifiable(this->getBase());
                    if (is.first) {
                        auto copy = *this;
                        copy.updateBase(std::move(is.second));
                        is.second = copy;
                    }
                }
            }
            else if (ee.IsInt() && (ee > omnn::math::constants::zero))
            {
                if (this->getBase().IsVa()) {
                    is = base::IsMultiplicationSimplifiable(v);
                } else if (this->getExponentiation().IsSimpleFraction()) {
                    auto recipExp = this->getExponentiation().as<Fraction>().Reciprocal();
                    is.second = v ^ recipExp;
                    is.first = is.second.MultiplyIfSimplifiable(this->getBase());
                    if (is.first) {
                        auto copy = *this;
                        copy.updateBase(std::move(is.second));
                        is.second = copy;
                    }
                } else if (this->getExponentiation().IsSimple()) {
                    is = base::IsMultiplicationSimplifiable(v);
                }
            }
        } else if (v.IsVa()) {
        } else if (v.IsProduct()) {
            is=v.IsMultiplicationSimplifiable(*this);
        } else if (v.IsSum()) {
            auto& sum=v.as<Sum>();
            is.first=sum==this->getBase()||-sum==this->getBase();
            if(is.first){
                is.second=*this*sum;
            }else{
                is=sum.IsMultiplicationSimplifiable(*this);
            }
        } else if (v.IsNaN()) {
            is = {true, v};
        } else if (v.IsPrincipalSurd()) {
            auto& surd = v.as<PrincipalSurd>();
            if (surd.Radicand() == getBase() && surd.Index().IsEven() == YesNoMaybe::No) {
                is = surd.Index().Reciprocal().IsSummationSimplifiable(getExponentiation());
                if (is.first) {
                    is.second = std::static_pointer_cast<Valuable>(
                        std::make_shared<Exponentiation>(getBase(), std::move(is.second)));
                }
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
            if (operator==(v))
                return *this;
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
            auto& e = v.as<omnn::math::Exponentiation>();
            if(this->ebase() == e.ebase() && (this->ebase().IsVa() || !this->ebase().IsMultival()))
            {
                this->eexp() -= e.eexp();
            }
            else
            {
                return this->Become(omnn::math::Fraction(*this, v));
            }
        }
        else if(v.IsFraction())
        {
            *this *= v.as<omnn::math::Fraction>().Reciprocal();
            return *this;
        }
        else if(this->ebase() == v)
        {
            --this->eexp();
        }
        else
        {
            return this->Become(omnn::math::Fraction(*this, v));
        }

        this->optimized = {};
        this->optimize();
        return *this;
    }

    Valuable& Exponentiation::operator^=(const Valuable& v)
    {
        this->eexp() *= v;
        this->optimized = {};
        this->optimize();
        return *this;
    }

    bool Exponentiation::operator==(const Exponentiation& other) const
    {
        return base::operator==(other);
    }

    bool Exponentiation::operator==(const Valuable& v) const
    {
        if (v.IsExponentiation()) {
            return operator==(v.as<omnn::math::Exponentiation>());
        }
        if (v.IsFraction()) {
            return (this->eexp().IsInt() || this->eexp().IsSimpleFraction())
                && this->eexp() < omnn::math::constants::zero
                && this->ebase() == (v.Reciprocal() ^ (-this->eexp()));
        }
        if (v.IsProduct() || v.IsSum()) {
            return v.operator==(*this);
        }
        return omnn::math::DuoValDescendant<Exponentiation>::operator==(v);
    }

    Exponentiation::operator double() const {
        return std::pow(static_cast<double>(ebase()), static_cast<double>(eexp()));
    }

    Valuable& Exponentiation::d(const Variable& x)
    {
        bool bhx = this->ebase().HasVa(x);
        bool ehx = this->eexp().HasVa(x);
        if(ehx) {
            auto lnBase = this->ebase().ln();
            auto expDeriv = this->eexp().d(x);
            auto baseDeriv = bhx ? this->ebase().d(x) : omnn::math::constants::zero;
            auto term1 = expDeriv * lnBase;
            auto term2 = bhx ? (this->eexp() * baseDeriv) / this->ebase() : omnn::math::constants::zero;
            return this->Become((*this) * (term1 + term2));
        } else if (bhx) {
            if(this->ebase() == x)
                return this->Become(this->eexp() * (this->ebase() ^ (this->eexp()-1)));
            else
                return this->Become(this->eexp() * (this->ebase() ^ (this->eexp()-1)) * this->ebase().d(x));
        }
        return this->Become(omnn::math::constants::zero);
    }

    Valuable& Exponentiation::integral(const Variable& x, const Variable& C) {
        if (ebase() == x) {
            auto& ee = eexp();
            if (ee.IsInt() || ee.IsSimpleFraction()) {
                updateExponentiation(ee + 1);
                *this /= eexp();
                return *this += C;
            }
            if (ee.IsSum()) {
                Product p;
                for (const auto& m : ee.as<Sum>()) {
                    p.Add(x ^ m);
                }
                return Become(std::move(p.integral(x, C)));
            }
        } else {
            // For non-variable base, use integration by substitution
            if (ebase().HasVa(x)) {
                // If base contains x, use u-substitution with u = base
                auto u = ebase();
                auto du = u.d(x);
                Become((u ^ eexp()) * du.integral(x, C));
            } else {
                // If base doesn't contain x, treat as constant coefficient
                Become((ebase() ^ eexp()) * x + C);
            }
        }
        return *this;
    }

    bool Exponentiation::operator <(const Valuable& v) const
    {
        if (v.IsExponentiation()) {
            auto& e = v.as<omnn::math::Exponentiation>();
            if (e.getBase() == this->getBase())
                return this->getExponentiation() < e.getExponentiation();
            if (e.getExponentiation() == this->getExponentiation())
                return this->getBase() < e.getBase();
        }
        return omnn::math::DuoValDescendant<Exponentiation>::operator<(v);
    }

    std::ostream& Exponentiation::print_sign(std::ostream& out) const {
        return out << "^";
    }

    bool Exponentiation::IsMultiSign() const {
        return eexp().IsFraction()
            && eexp().template as<Fraction>().getDenominator().IsEven() == YesNoMaybe::Yes;
    }

    YesNoMaybe Exponentiation::IsMultival() const {
        auto is = ebase().IsMultival() || eexp().IsMultival();
        if (is != YesNoMaybe::Yes && eexp().IsFraction())
            is = eexp().template as<Fraction>().getDenominator().IsEven() || is;
        return is;
    }

    void Exponentiation::Values(const std::function<bool(const Valuable&)>& fun) const {
        if (fun) {
            auto cache = optimized;
            solutions_t vals;
            std::deque<Valuable> d1;

            auto baseValuesHandler = [&](const Valuable& v) -> bool {
                d1.push_back(v);
                return true;
            };
            ebase().Values(baseValuesHandler);

            auto expValuesHandler = [&](const Valuable& v) -> bool {
                auto vIsFrac = v.IsFraction();
                const Fraction* f = nullptr;
                if(vIsFrac)
                    f = &v.template as<Fraction>();
                auto vMakesMultival = vIsFrac && f->getDenominator().IsEven()==YesNoMaybe::Yes;

                for(auto& item1 : d1) {
                    if(vMakesMultival) {
                        static const Variable x;
                        auto& dn = f->getDenominator();
                        auto solutions = (x ^ dn).Equals(*this ^ dn).Solutions(x);
                        for(auto&& s : solutions)
                            vals.insert(s);
                    } else {
                        auto value = item1 ^ v;
                        if(value.IsMultival()==YesNoMaybe::No)
                            vals.insert(value);
                        else {
                            value.Values([&](const Valuable& subValue) -> bool {
                                vals.insert(subValue);
                                return true;
                            });
                        }
                    }
                }
                return true;
            };
            eexp().Values(expValuesHandler);

            for(const auto& v : vals)
                fun(v);
        }
    }

    std::ostream& Exponentiation::code(std::ostream& out) const {
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

    bool Exponentiation::IsComesBefore(const Exponentiation& e) const {
        bool is = {};
        bool baseIsVa = ebase().IsVa();
        bool vbaseIsVa = e.ebase().IsVa();
        if (baseIsVa && vbaseIsVa)
            is = eexp() == e.eexp() ? ebase().IsComesBefore(e.ebase())
                                   : eexp() > e.eexp();
        else if (baseIsVa)
            is = false;
        else if (vbaseIsVa)
            is = true;
        else if (ebase() == e.ebase())
            is = eexp().IsComesBefore(e.eexp());
        else if (eexp() == e.eexp())
            is = ebase().IsComesBefore(e.ebase());
        else {
            auto c = Complexity();
            auto ec = e.Complexity();
            if (c != ec)
                is = c > ec;
            else {
                is = ebase().IsComesBefore(e.ebase()) ||
                     (!e.ebase().IsComesBefore(ebase()) &&
                      eexp().IsComesBefore(
                          e.eexp())); //  || str().length() > e->str().length();
                //                auto expComesBefore = eexp().IsComesBefore(e->eexp());
                //                auto ebase()ComesBefore = ebase().IsComesBefore(e->ebase());
                //                is = expComesBefore==ebase()ComesBefore || str().length() > e->str().length();
            }
        }
        return is;
    }

    bool Exponentiation::IsComesBefore(const Valuable& value) const
    {
        if (value.IsExponentiation()) {
            return IsComesBefore(value.as<omnn::math::Exponentiation>());
        }

        bool is = FindVa() && value.IsSimple();
        if (is)
        {
            is = false;
        }
        else if(value.IsProduct())
        {
            is = !(value.IsComesBefore(*this) || this->operator==(value));
        }
        else
        {
            // MSVC compatibility: move init-statement outside if
            auto degreeDiff = this->getMaxVaExp() - value.getMaxVaExp();
            if (degreeDiff != omnn::math::constants::zero)
            {
                is = degreeDiff > omnn::math::constants::zero;
            }
            else if (value.IsSimple())
                is = true;
//        else if(v.IsFraction())
//        {is=}
            else if(value.IsVa())
                is = FindVa() != nullptr;
            else if(value.IsSum())
                is = IsComesBefore(*value.as<omnn::math::Sum>().begin());
            else {
                // For any other type, compare based on type precedence
                is = typeid(*this).before(typeid(value));
            }
        }

        return is;
    }

    Valuable Exponentiation::calcFreeMember() const
    {
        Valuable c;
        if(ebase().IsSum() && eexp().IsInt()){
            c = ebase().calcFreeMember() ^ eexp();
        } else if(ebase().IsVa()) {
            c = 0_v;
        } else {
            // For non-sum, non-variable bases, calculate the free member
            // by evaluating the expression with all variables set to zero
            c = ebase().calcFreeMember() ^ eexp();
        }
        return c;
    }

    Valuable& Exponentiation::reciprocal() {
        Valuable::hash ^= eexp().Hash();
        eexp() = -eexp();
        Valuable::hash ^= eexp().Hash();
        return *this;
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
        return Become(PrincipalSurd(*this, 2));
    }

    const Valuable::vars_cont_t& Exponentiation::getCommonVars() const {
#ifndef NDEBUG
        auto& b = this->ebase();
        if (b.IsVa()) { // TODO: FindVa too
            auto& va = b.template as<Variable>();
            auto it = this->v.find(va);
            if (it != this->v.end() && !it->second.Same(this->eexp())) {
                for (auto& p : this->v) {
                    std::cout << p.first << " -> " << p.second << std::endl;
                }

                //<< (v.begin()->first == b)
                LOG_AND_IMPLEMENT(*this << " Exponentiation::getCommonVars not ready, no " << b << " in v ");
            }
        }
#endif
        return this->v;
    }

    Valuable::vars_cont_t Exponentiation::GetVaExps() const
    {
        auto vaExps = ebase().GetVaExps();
        auto& e = eexp();
        for (auto& ve : vaExps) {
            ve.second *= e;
        }
        return vaExps;
    }

    Valuable Exponentiation::InCommonWith(const Valuable& v) const
    {
        auto c = omnn::math::constants::one;
        if (v.IsProduct()) {
            for(auto& m: v.template as<Product>()){
                c = InCommonWith(m);
                if (c != omnn::math::constants::one) {
                    break;
                }
            }
        } else if (v.IsExponentiation()) {
            auto& e = v.template as<omnn::math::Exponentiation>();
            if (e.ebase() == this->ebase()) {
                if (e.eexp() == this->eexp()) {
                    c = e;
                } else if (this->eexp().IsSimple() && e.eexp().IsSimple()) {
                    if (this->eexp() > 0 || e.eexp() > 0) {
                        if (e.eexp() >= this->eexp()) {
                            c = *this;
                        } else
                            c = e;
                    } else if (this->eexp() < 0 || e.eexp() < 0) {
                        if (e.eexp() >= this->eexp()) {
                            c = e;
                        } else
                            c = *this;
                    } else {
                        c = omnn::math::constants::one;
                    }
                } else if (this->eexp().IsSimpleFraction() && e.eexp().IsSimpleFraction()) {
                    if (this->eexp()<0 == e.eexp()<0) {
                        c = this->ebase() ^ this->eexp().InCommonWith(e.eexp());
                    }
                } else if (this->eexp().IsSum()) {
                    auto sz = this->eexp().template as<Sum>().size();
                    auto diff = this->eexp() - e.eexp();
                    if (!diff.IsSum() || diff.template as<Sum>().size() < sz)
                        c = v;
                } else if (e.eexp().IsSum()) {
                    c = e.InCommonWith(*this);
                } else if (e.eexp().IsProduct()) {
                    c = this->ebase() ^ e.eexp().InCommonWith(this->eexp());
                } else {
                    auto commonExp = this->eexp().InCommonWith(e.eexp());
                    c = this->ebase() ^ commonExp;
                }
            }
        } else if (this->eexp().IsInt()) {
            if(this->eexp() > 0)
                c = this->ebase().InCommonWith(v);
        } else if (this->eexp().IsFraction()) {
            c = this->ebase().InCommonWith(v);
        } else if (v.IsVa()) {
            c = v.InCommonWith(*this);
        } else if (v.IsInt() || v.IsSimpleFraction()) {
            c = this->ebase().InCommonWith(v);
        } else if (v.IsModulo()) {
            c = this->ebase().InCommonWith(v.template as<Modulo>().getBase());
        } else if (this->eexp().IsVa()) {
            c = this->ebase().InCommonWith(v);
        } else if (this->eexp().IsModulo()) {
            c = this->ebase().InCommonWith(v) ^ this->eexp().template as<Modulo>().getBase();
        } else if (v.IsPrincipalSurd()) {
            auto commonWithBase = v.InCommonWith(this->ebase());
            if (commonWithBase != omnn::math::constants::one) {
                return commonWithBase ^ this->eexp();
            }
        } else if (v.IsSum() || v.IsProduct()) {
            for (const auto& term : v.IsSum() ? v.template as<Sum>() : v.template as<Product>()) {
                auto commonTerm = InCommonWith(term);
                if (commonTerm != omnn::math::constants::one) {
                    c = commonTerm;
                    break;
                }
            }
        } else {
            c = omnn::math::constants::one;
        }
        return c;
    }

    Valuable Exponentiation::operator()(const Variable& va) const
    {
        return operator()(va, constants::zero);
    }

    Valuable Exponentiation::operator()(const Variable& v, const Valuable& augmentation) const
    {
        if (!eexp().FindVa() && eexp()!=0 && augmentation==0) {
            return ebase()(v,augmentation);
        } else if (eexp().IsSimpleFraction()) {
            auto& f = eexp().as<Fraction>();
            return (ebase()^f.getNumerator())(v,augmentation^f.getDenominator());
        } else {
            return (ebase()(v, augmentation)) ^ eexp();
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
                        // Handle complex roots for higher even denominators
                        auto roots = exponentiationBase.Sqrt();
                        for (auto& root : roots.Distinct()) {
                            auto branchDistinct = (root ^ (f.numerator() * 2)).Distinct();
                            branches.insert(branchDistinct.begin(), branchDistinct.end());
                        }
                    }
                } else {
                    OptimizeOn optimizeOn;
                    auto b = ebase() ^ f.numerator();
                    auto d = denom;
                    do {
                        b.sqrt();
                        d.shr();
                    } while(d.IsEven() == YesNoMaybe::Yes);
                    auto _ = b ^ d.Reciprocal();
                    auto distinct = _.Distinct();
                    if(denom==constants::two)
                        for (auto& branch : distinct)
                        {
                            branches.emplace(-branch);
                            branches.emplace(std::move(const_cast<decltype(distinct)::reference>(branch)));
                        }
                    else if(denom==4)
                        for (auto& branch : distinct)
                        {
                            branches.emplace(-branch * omnn::math::constants::i);
                            branches.emplace(branch * omnn::math::constants::i);
                            branches.emplace(-branch);
                            branches.emplace(std::move(const_cast<decltype(distinct)::reference>(branch)));
                        }
                    else
                    {
                        auto Branch = [](auto&& collection) -> solutions_t {
                            solutions_t branches;
                            auto e = collection.end();
                            for (auto it = collection.begin(); it != e;) {
                                auto item = std::move(collection.extract(it++).value());
                                item.sqrt();
                                branches.emplace(-item);
                                branches.emplace(std::move(item));
                            }
                            return branches;
                        };

                        branches = std::move(distinct);
                        for(auto d = denom; d.IsEven() == YesNoMaybe::Yes; d.shr()) {
                            auto newBranches = Branch(branches);
                            branches = std::move(newBranches);
                        }

                        if (d != constants::one) {
                            auto nthRoot = b ^ d.Reciprocal();
                            auto rootDistinct = nthRoot.Distinct();
                            branches.insert(rootDistinct.begin(), rootDistinct.end());
                        }
                    }
                }
            }
        } else {
            branches.emplace(*this);
        }
        return branches;
    }

    bool Exponentiation::IsPolynomial(const Variable& v) const {
        return this->IsVaExp()
            && this->eexp().IsInt()
            && this->eexp() > omnn::math::constants::zero;
    }

    void Exponentiation::solve(const Variable& va, solutions_t& s) const {
        if (this->ebase() == va
            && !this->eexp().FindVa()
            && this->eexp() != omnn::math::constants::zero)
        {
            s.emplace(omnn::math::constants::zero);
        } else {
            if (this->ebase().FindVa()) {
                auto baseEq = this->ebase().Equals(va);
                s.insert(baseEq.Solutions(va).begin(), baseEq.Solutions(va).end());
            } else if (this->eexp().FindVa()) {
                auto logEq = this->eexp() * this->ebase().Log();
                s.insert(logEq.Solutions(va).begin(), logEq.Solutions(va).end());
            }
        }
    }

    Valuable Exponentiation::Sign() const {
        return this->ebase().Sign() ^ this->eexp();
    }

} // namespace omnn::math
