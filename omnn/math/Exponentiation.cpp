//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Exponentiation.h"

#include "Euler.h"
#include "i.h"
#include "Infinity.h"
#include "pi.h"
#include "Integer.h"
#include "NaN.h"
#include "PrincipalSurd.h"
#include "Product.h"
#include "Sum.h"

#include <cmath>
#include <limits>

#include <boost/numeric/conversion/cast.hpp>

using namespace omnn::math;

    max_exp_t Exponentiation::getMaxVaExp(const Valuable& b, const Valuable& e)
    {
        if (e.IsInt()) {
            return b.getMaxVaExp() * e.ca();
        } else if (e.FindVa()) {
            auto _ = e * b.getMaxVaExp();
            _.eval(e.GetVaExps());
            OptimizeOn(), _.optimize();
            return static_cast<decltype(maxVaExp)>(_);
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
        if (ebase().IsVa()) {
            v[ebase().template as<Variable>()] = eexp();
        }
    }

    Valuable& Exponentiation::optimize()
    {
        if (is_optimized() || !optimizations)
            return *this;

        DUO_OPT_PFX

        if (IsEquation()) {
            Become(std::move(ebase()));
            return optimize();
        }

        ebase().optimize();
        eexp().optimize();

        if (ebase().IsExponentiation() || ebase().IsProduct()) {
            ebase() ^= eexp();
            Become(std::move(ebase()));
            return optimize();
        }

        if (eexp().IsSum()) {
            auto& s = eexp().template as<Sum>();
            auto sz = s.size();
            Valuable v = 1;
            for (auto it = s.begin(); it != s.end();) {
                v *= *this ^ *it;
                ++it;
            }
            if (sz != s.size()) {
                Become(*this * v);
                return optimize();
            }
        }

        if (ebase().IsSum()) {
            auto& s = ebase().template as<Sum>();
            if (eexp().IsInt() && eexp() > 0) {
                auto lcm = s.LCMofMemberFractionDenominators();
                if (lcm != constants::one) {
                    ebase() *= lcm;
                    Become(*this * lcm);
                    return optimize();
                }
            }
        }

        if (ebase().IsExponentiation() || ebase().IsProduct())
        {
            ebase() ^= eexp();
            Become(std::move(ebase()));
            return optimize();
        }

        if (eexp().IsSum())
        {
            auto& s = eexp().as<Sum>();
            auto sz = s.size();
            auto v = constants::one;
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
                return optimize();
            }
        }

        if (ebase().IsSum())
        {
            if (eexp() == constants::minus_1) {
                auto& s = ebase().as<Sum>();
				auto lcm = s.LCMofMemberFractionDenominators();
                if (lcm != constants::one) {
                    ebase() *= lcm;
				    Become(*this * lcm);
                    return optimize();
                }
            }
			//auto& s = ebase().as<Sum>();
			//auto sz = s.size();
			//auto v = 1_v;
   //         for (auto it = s.begin(), e = s.end();
   //             it != e; )
   //         {
   //             if (it->IsInt()) {
			//		v *= *it^eexp();
			//		s.Delete(it);
			//	}
			//	else
			//		++it;
			//}
   //         if (sz != s.size()) {
			//	Become(*this * v);
			//	return;
			//}
		}

  //      if (ebase().IsExponentiation() && eexp().IsExponentiation())
  //      {
		//	auto& e = ebase().as<Exponentiation>();
		//	auto& ee = eexp().as<Exponentiation>();
  //          if (e.getBase() == ee.getBase())
  //          {
		//		eexp() *= ee.getExponentiation();
		//		ebase() = e.getBase();
		//		optimized = {};
		//		optimize();
		//		return;
		//	}
		//}

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

        if (ebase().IsInt() && eexp().IsSimpleFraction()) {
            auto& f = eexp().template as<Fraction>();
            if (f.getNumerator() == 1 && f.getDenominator() == 2) {
                auto base = ebase().ca();
                if (base >= 0) {
                    auto value = boost::multiprecision::sqrt(base);
                    if (value * value == base) {
                        if (base == 0) {
                            Become(Integer(0));
                        } else {
                            solutions_t result;
                            result.insert(Integer(value));
                            if (base != 1) {  // Only add negative root if not 1
                                result.insert(Integer(-value));
                            }
                            Become(Valuable(std::move(result)));
                            SetView(View::Calc);
                            SetMultival(result.size() > 1 ? YesNoMaybe::Yes : YesNoMaybe::No);
                        }
                        return optimize();
                    }
                }
            }
        }
        
        if (ebase().IsFraction() && eexp().IsMultival()==YesNoMaybe::No) {
            auto& f = ebase().template as<Fraction>();
            auto _ = (f.getNumerator() ^ eexp()) / (f.getDenominator() ^ eexp());
            if (_.IsExponentiation()) {
                auto& e = _.template as<Exponentiation>();
                if (!(e.ebase()==ebase() && eexp()==e.eexp())) {
                    return *this;
                }
            } else {
                Become(std::move(_));
                return optimize();
            }
        }

        if (ebase().IsFraction() && eexp().IsInt() && eexp() < constants::zero) {
            eexp() = -eexp();
            ebase() = ebase().template as<Fraction>().Reciprocal();
        }

		// e^(i*pi) = -1
                // it is a fundamental equation that gives us a hint on cross-dimmensional relations
                // because i and -1 are 1 of different signs/dimmensions
        if (ebase().Is_e()) {
            if (eexp().IsProduct()) {
                auto& p = eexp().template as<Product>();
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
                    return *this;
                }
            }
        }
        // todo : check
        if (ebase().IsSimple()) {
            if (eexp().IsProduct()) {
                auto& p = eexp().as<Product>();
                auto it = p.GetFirstOccurence<Integer>();
                if (it != p.end()) {
                    auto in = ebase() ^ *it; // todo : IsExponentiationSimplifiable(p.begin())
                    if (in.IsInt()) {
                        ebase() = in;
                        p.Delete(it);
                        if (p.size() == 1) {
                            eexp().optimize();
                        }
                    }
                }
            }
            if (ebase() == constants::one) {

                if (eexp().IsSimple())
                {
                    if (eexp() < constants::zero) {
                        setExponentiation(-eexp());
                    }
                    else if (eexp().IsZero()) {
                        Become(std::move(ebase()));
                        return optimize();
                    }
                }

                if (eexp().IsInt()) {
                    Become(std::move(ebase()));
                    return optimize();
                } else if (eexp().IsSimpleFraction() && eexp() > constants::zero) {
                    auto& f = eexp().as<Fraction>();
                    auto& n = f.getNumerator();
                    if (n.IsEven() == YesNoMaybe::Yes) {
                        auto&& toOptimize = std::move(eexp());
                        toOptimize.optimize();
                        setExponentiation(std::move(toOptimize));
                        return optimize();
                    }
                    auto& dn = f.getDenominator();
                    if (dn.bit()) {
                        Become(std::move(ebase()));
                        return optimize();
                    } else if (n != constants::one) {
                        hash ^= f.Hash();
                        f.update1(1_v);
                        hash ^= f.Hash();
					}
                } else if (!!eexp().IsMultival()) {
                } else if (!(eexp().IsInfinity() || eexp().IsMInfinity())) {
                    Become(std::move(ebase()));
                    return optimize();
                } else {
                    Become(NaN());
                    return optimize();
                }
            } else if (ebase() == -1 && eexp().IsInt() && eexp() > 0 && eexp() != 1) {
                    eexp() = eexp().bit(0);
            } else if (eexp()==-1) {
                Become(Fraction{1,ebase()});
                return optimize();
            } else if (eexp().IsInfinity()) {
                Become(NaN());
                return optimize();
            } else if (eexp().IsFraction()) {
                auto& f = eexp().as<Fraction>();
                auto& n = f.getNumerator();
                if (n > constants::one) {
                    auto newBase = ebase() ^ n;
                    if(!newBase.IsExponentiation()){
                        Become(newBase ^ f.getDenominator().Reciprocal());
                        return optimize();
                    }
                }
            }
        }

        bool ebz = ebase().IsZero();
        bool exz = eexp().IsZero();
        if(exz)
        {
            // When exponent is zero:
            // 1. Return NaN for infinite base (IEEE 754 compliance)
            // 2. Return NaN for zero base (0^0 is undefined)
            if (ebase().IsInfinity() || ebase().IsMInfinity() || ebz) {
                Become(NaN{});
                return optimize();
            }

            // x^0 = 1 for all other x
            Become(1_v);
            return optimize();
        }
        else if(eexp() == constants::one)
        {
            Become(std::move(ebase()));
            return optimize();
        }
        else if (ebz)
        {
            if (exz) {
                Become(NaN{});
                return optimize();
            }
            Become(0_v);
            return optimize();
        }
        else if (ebase().IsInfinity())
        {
            if (eexp() > 0) { // FIXME: ^(1/2) = ±Infinity
                Become(std::move(ebase()));
            } else if (eexp() < 0) {
                Become(0_v);
            } else {
                Become(NaN{});
            }
            return optimize();
        }
        else if (ebase().IsMInfinity())
        {
            if (eexp().IsZero()) {
                Become(NaN{});
                return optimize();
            } else if (eexp() > 0) {
                if ((eexp() % 2) > 0) // TODO : test with non-ints
                    Become(std::move(ebase()));
                else
                    Become(Infinity());
                return optimize();
            } else {
                Become(NaN());
                return optimize();
            }
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
                        if (ebase().IsInt()) {
                            auto base = ebase().ca();
                            auto exp = eexp().ca();
                            auto result = base;
                            for (auto i = 1; i < exp; ++i) {
                                result *= base;
                            }
                            Become(Integer(result));
                            return optimize();
                        } else {
                            auto b = constants::one;
                            for (; eexp()--;) {
                                b *= ebase();
                            }
                            Become(std::move(b));
                            return optimize();
                        }
                    }
                    if (ebase().IsInt() && eexp().IsInt()) {
                        Become(ebase() ^ eexp());
                        return optimize();
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
                                if (n.IsZero())
                                {
                                    Become(1_v);
                                    return optimize();
                                }
                                auto y = constants::one;
                                while (n > constants::one)
                                {
                                    bool isInt = n.IsInt();
                                    if (!isInt) {
                                        Become(NaN());
                                        return optimize();
                                    }
                                    if (isInt && n.bit().IsZero())
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
                            if (ebase().IsZero())
                            {
                                Become(NaN());
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
                case View::Fraction: {
                    if (eexp().IsSimple() && eexp() < constants::zero) {
                        updateExponentiation(-eexp());
                        Become(Fraction{constants::one, std::move(ebase().reciprocal())});
                    }
                    break;
                }
                default: {
                    Become(NaN());
                    return optimize();
                }
            }
            if (IsEquation()) {
                if (eexp().IsSimple())
                    Become(std::move(ebase()));
            }
        }

        if (IsExponentiation() && ebase().IsExponentiation())
        {
            auto& e = ebase().template as<Exponentiation>();
            auto& eeexp = e.getExponentiation();
            if ((eeexp.FindVa() == nullptr) == (eexp().FindVa() == nullptr)) {
                eexp() *= eeexp;
                ebase() = std::move(const_cast<Valuable&>((e.getBase())));
            }

            if (eexp() == constants::one || IsEquation()) {
                Become(std::move(ebase()));
                return optimize();
            }
        }

        if (IsExponentiation()) {
            hash = ebase().Hash() ^ eexp().Hash();
            InitVars();
            MarkAsOptimized();
        }
        return *this;
    }

    Valuable& Exponentiation::operator +=(const Valuable& v)
    {
        Become(Sum {*this, v});
        return optimize();
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
            && (eexp().IsInt() || eexp().IsSimpleFraction())
            && (e->eexp().IsInt() || e->eexp().IsSimpleFraction())
            )
        {
            updateExponentiation(eexp() + e->getExponentiation());
            optimized = {};
            return optimize();
        }
        else if(v.IsFraction()
                && (f = &v.template as<Fraction>())->getDenominator() == ebase())
        {
            --eexp();
            optimized={};
            optimize();
            *this *= f->getNumerator();
            optimized = {};
            return optimize();
        }
        else if(v.IsFraction()
                && f->getDenominator().IsProduct()
                && (fdn = &f->getDenominator().template as<Product>())->Has(ebase()))
        {
            --eexp();
            optimized={};
            optimize();
            auto result = *this *= f->getNumerator() / (*fdn / ebase());
            optimized = {};
            return optimize();
        }
        else if(fdn
                && (fdne = isProdHasExpWithSameBase(fdn)))
        {
            eexp() -= fdne->getExponentiation();
            optimized={};
            optimize();
            auto result = *this *= f->getNumerator() / (*fdn / *fdne);
            optimized = {};
            return optimize();
        }
        else if(b == v && v.FindVa())
        {
            updateExponentiation(eexp()+1);
            optimized = {};
            return optimize();
        }
        else if(-b == v && v.FindVa())
        {
            updateExponentiation(eexp()+1);
            auto result = Become(Product{-1, *this});
            optimized = {};
            return optimize();
        }
        else if(v.IsProduct())
        {
            auto result = Become(v * *this);
            optimized = {};
            return optimize();
        }
        else if(v.IsInt())
        {
            if(v==1) {
                optimized = {};
                return optimize();
            }
            else if(eexp()==-1 && ebase().IsInt()) {
                auto result = Become(v/ebase());
                optimized = {};
                return optimize();
            }
            else {
                auto result = Become(Product{v, *this});
                optimized = {};
                return optimize();
            }
        }
        else {
            auto result = Become(Product{v, *this});
            optimized = {};
            return optimize();
        }
    }

    bool Exponentiation::MultiplyIfSimplifiable(const Integer& integer) {
        bool is = {};
        if (integer == constants::one) {
            is = true;
        } else if (integer.IsZero()) {
            Become(0);
            optimize();
            is = true;
        } 

        return is;
    }

    bool Exponentiation::MultiplyIfSimplifiable(const Valuable& v)
    {
        auto is = v == getBase();
        if (is) {
            ++eexp();
            optimized = {};
            optimize();
        } else if (v.IsInt()) {
            is = MultiplyIfSimplifiable(v.as<Integer>());
        } else if (!FindVa() && IsMultival() == YesNoMaybe::Yes && !v.FindVa()) {
            solutions_t values;
            auto distinct = Distinct();
            while(distinct.size()) {
                auto extract = std::move(distinct.extract(distinct.begin()).value());
                if (extract.MultiplyIfSimplifiable(v)) {
                    is = true;
                } else {
                    extract *= v;
                }
                values.emplace(std::move(extract));
            }
            Become(Valuable(std::move(values)));
            SetMultival(values.size() > 1 ? YesNoMaybe::Yes : YesNoMaybe::No);
            optimize();
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
            auto& vexpo = v.template as<Exponentiation>();
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
                auto gcd = ebase().GCD(v);
                is.first = gcd != constants::one;
                if(is.first){
                    is.second = (v/gcd)/(ebase()/gcd);
                } else {
                    is.second = v ^ constants::minus_1; // v.IsExponentiationSimplifiable(ee)
                    is.first = is.second.MultiplyIfSimplifiable(getBase());
                    if (is.first) {
                        auto copy = *this;
                        copy.updateBase(std::move(is.second));
                        is.second = copy;
                    }
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
            auto& sum=v.template as<Sum>();
            is.first=sum==getBase()||-sum==getBase();
            if(is.first){
                is.second=*this*sum;
            }else{
                is=sum.IsMultiplicationSimplifiable(*this);
            }
        } else if (v.IsNaN()) {
            is = {true, v};
        } else if (v.IsPrincipalSurd()) {
            auto& surd = v.template as<PrincipalSurd>();
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
            if (is) {
                Become(std::move(sumIfSimplifiable.second));
                optimized = {};
                optimize();
            }
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

    Valuable& Exponentiation::operator/=(const Valuable& v)
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

            Become(Valuable(std::move(vals)));
            optimized = {};
            return optimize();
        }
        else if (v.IsExponentiation())
        {
            auto& e = v.template as<Exponentiation>();
            if(ebase() == e.ebase() && (ebase().IsVa() || !ebase().IsMultival()))
            {
                eexp() -= e.eexp();
                optimized = {};
                optimize();
                return *this;
            }
            else
            {
                Become(Fraction(*this, v));
                optimized = {};
                optimize();
                return *this;
            }
        }
        else if(v.IsFraction())
        {
            *this *= v.template as<Fraction>().Reciprocal();
            optimized = {};
            optimize();
            return *this;
        }
        else if(ebase() == v)
        {
            --eexp();
            optimized={};
            optimize();
            return *this;
        }
        else
        {
            Become(Fraction(*this, v));
            optimized = {};
            optimize();
            return *this;
        }
    }

    Valuable& Exponentiation::operator^=(const Valuable& v)
    {
        eexp() *= v;
        optimized = {};
        return optimize();
    }

    bool Exponentiation::operator==(const Exponentiation& other) const {
        return base::operator ==(other);
    }

    bool Exponentiation::operator==(const Valuable& v) const {
        if (v.IsExponentiation()) {
            return operator==(v.template as<Exponentiation>());
        }
        if (v.IsFraction()) {
            if (!(eexp().IsInt() || eexp().IsSimpleFraction()) || !(eexp() < 0)) {
                return false;
            }
            auto reciprocal = v.template as<Fraction>().Reciprocal();
            reciprocal.optimize();
            auto negExp = -eexp();
            negExp.optimize();
            auto powered = reciprocal ^ negExp;
            powered.optimize();
            return ebase() == powered;
        }
        if (v.IsProduct() || v.IsSum()) {
            return operator==(v);
        }
        return false;
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
            if(bhx) {
                auto lnf = Logarithm(ebase(), Valuable(1));
                auto gp = eexp().d(x);
                auto fp = ebase().d(x);
                Become(*this * (gp * lnf + eexp() * fp / ebase()));
            } else {
                auto lna = Logarithm(ebase(), Valuable(1));
                auto gp = eexp().d(x);
                Become(*this * lna * gp);
            }
        } else if (bhx) {
            if(ebase() == x) {
                Become(eexp() * (ebase() ^ (eexp()-1)));
            } else {
                Become(NaN());
            }
        } else {
            Become(0_v);
        }
        optimized = {};
        return optimize();
    }

    Valuable& Exponentiation::integral(const Variable& x, const Variable& C)
    {
        if (ebase() != x) {
            optimized = {};
            return optimize();
        }
        
        const auto& ee = eexp();
        if (ee.IsSum()) {
            Product p({});
            for (const auto& m : ee.template as<Sum>()) {
                p.Add(x ^ m);
            }
            Become(std::move(p.integral(x, C)));
            optimized = {};
            return optimize();
        } else if (ee.IsInt() || ee.IsSimpleFraction()) {
            auto newExp = ee + 1;
            auto result = x ^ newExp;
            result /= ee;
            result += C;
            Become(std::move(result));
            optimized = {};
            return optimize();
        } else {
            Become(NaN());
            optimized = {};
            return optimize();
        }
    }

    bool Exponentiation::operator<(const Valuable& v) const 
    {
        if (!v.IsExponentiation()) {
            return base::operator<(v);
        }
        
        const auto& e = v.template as<Exponentiation>(); 
        // Compare components to avoid recursive comparison
        auto baseCompare = getBase().Hash() - e.getBase().Hash();
        if (baseCompare == 0) {
            return getExponentiation().Hash() < e.getExponentiation().Hash();
        }
        return baseCompare < 0;
    }

    std::ostream& Exponentiation::print_sign(std::ostream& out) const
    {
        return out << "^";
    }

    bool Exponentiation::IsMultiSign() const {
        return _2.IsFraction()
            && _2.as<Fraction>().getDenominator().IsEven() == YesNoMaybe::Yes;
    }
    YesNoMaybe Exponentiation::IsMultival() const
    {
        auto is = _1.IsMultival() || _2.IsMultival();
        if (is != YesNoMaybe::Yes && _2.IsFraction())
            is = _2.as<Fraction>().getDenominator().IsEven() || is;
        return is;
    }

    YesNoMaybe Exponentiation::IsRational() const { // FIXME: no Yes scenarios
        return base::IsRational() &&
               (IsMultival() == YesNoMaybe::Yes ? YesNoMaybe::No : YesNoMaybe::Maybe);
    }

    Valuable& Exponentiation::Values(const std::function<bool(const Valuable&)>& fun) const {
        if (!fun) return const_cast<Valuable&>(static_cast<const Valuable&>(*this));

        solutions_t vals;
        std::deque<Valuable> d1;
        _1.Values([&](const auto& v){
            d1.push_back(v);
            return true;
        });

        _2.Values([&](const auto& v){
            auto vIsFrac = v.IsFraction();
            const Fraction* f = vIsFrac ? &v.template as<Fraction>() : nullptr;
            auto vMakesMultival = vIsFrac && f && f->getDenominator().IsEven() == YesNoMaybe::Yes;

            for(const auto& item1 : d1){
                if(vMakesMultival){
                    static const Variable x;
                    auto& dn = f->getDenominator();
                    auto powered = x ^ dn;
                    powered.optimize();
                    auto thisPowered = *this ^ dn;
                    thisPowered.optimize();
                    auto solutions = powered.Equals(thisPowered).Solutions(x);
                    for(auto&& s : solutions) {
                        auto optimized_s = std::move(s);
                        optimized_s.optimize();
                        vals.insert(std::move(optimized_s));
                    }
                } else {
                    auto value = item1^v;
                    value.optimize();
                    if(value.IsMultival() == YesNoMaybe::No) {
                        vals.insert(std::move(value));
                    }
                }
            }
            return true;
        });

        for(const auto& v : vals) {
            if (!fun(v)) break;
        }
        
        return const_cast<Valuable&>(static_cast<const Valuable&>(*this));
    }

    std::ostream& Exponentiation::code(std::ostream& out) const
    {
        auto exp = getExponentiation();
        exp.optimize();
        if (exp.IsInt()) {
            out << "(1";
            for (auto i = exp; i-- > 0;) {
                out << '*';
                auto base = getBase();
                base.optimize();
                base.code(out);
            }
            out << ')';
        } else {
            out << "pow(";
            auto base = getBase();
            base.optimize();
            base.code(out) << ',';
            exp.code(out) << ')';
        }
        return out;
    }

    bool Exponentiation::IsComesBefore(const Exponentiation& e) const {
        bool is = {};
        bool baseIsVa = getBase().IsVa();
        bool vbaseIsVa = e.getBase().IsVa();
        if (baseIsVa && vbaseIsVa)
            is = getExponentiation() == e.getExponentiation() ? getBase().IsComesBefore(e.getBase())
                                                              : getExponentiation() > e.getExponentiation();
        else if (baseIsVa)
            is = false;
        else if (vbaseIsVa)
            is = true;
        else if (getBase() == e.ebase())
            is = getExponentiation().IsComesBefore(e.getExponentiation());
        else if (getExponentiation() == e.getExponentiation())
            is = getBase().IsComesBefore(e.getBase());
        else {
            auto c = Complexity();
            auto ec = e.Complexity();
            if (c != ec)
                is = c > ec;
            else {
                is = getBase().IsComesBefore(e.getBase()) ||
                     (!e.ebase().IsComesBefore(ebase()) &&
                      getExponentiation().IsComesBefore(
                          e.getExponentiation())); //  || str().length() > e->str().length();
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
            return IsComesBefore(value.template as<Exponentiation>());
        }

        auto is = FindVa() && value.IsSimple();
        if (is)
        {
            is = {};
        }
        else if(value.IsProduct())
        {
            is = !(value.IsComesBefore(*this) || operator==(value));
        }
        else if (auto degreeDiff = getMaxVaExp() - value.getMaxVaExp(); degreeDiff != 0)
        {
            is = degreeDiff > 0;
        }
        else if (value.IsSimple())
            is = true;
//        else if(v.IsFraction())
//        {is=}
        else if(value.IsVa())
            is = !!FindVa();
        else if(value.IsSum())
            is = IsComesBefore(*value.template as<Sum>().begin());
        else
            is = false;

        return is;
    }

    Valuable Exponentiation::calcFreeMember() const {
        if (getBase().IsSum() && getExponentiation().IsInt()) {
            auto c = getBase().calcFreeMember() ^ getExponentiation();
            c.optimize();
            return c;
        } else if (getBase().IsVa()) {
            return 0_v;
        }
        return NaN();
    }

    Valuable& Exponentiation::reciprocal() {
        auto oldHash = eexp().Hash();
        eexp() = -eexp();
        Valuable::hash ^= oldHash ^ eexp().Hash();
        optimized = {};
        return optimize();
    }

    Valuable& Exponentiation::sq()
    {
        eexp() *= 2;
        optimized = {};
        return optimize();
    }

    Valuable Exponentiation::Sqrt() const {
        Valuable copy = *this;
        copy.sqrt();
        copy.optimize();
        return std::move(copy);
    }

    Valuable& Exponentiation::sqrt() {
        Become(PrincipalSurd(*this, 2));
        optimized = {};
        return optimize();
    }

    const Valuable::vars_cont_t& Exponentiation::getCommonVars() const {
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
                LOG_AND_IMPLEMENT(*this << " Exponentiation::getCommonVars not ready, no " << b << " in v ");
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
                if (c != constants::one) {
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
                        c = NaN();
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
                } else if (getExponentiation().IsPrincipalSurd()) {
                    auto& surd = getExponentiation().as<PrincipalSurd>();
                    auto isSumSimpl = e.getExponentiation().IsSummationSimplifiable(-surd);
                    if (isSumSimpl.first) {
                        c = getBase() ^ isSumSimpl.second;
                    } else {
                        c = surd.InCommonWith(e.getExponentiation());
                        if (c != constants::one) {
                            c = Exponentiation{getBase(), std::move(c)}; // NOTE: this is not strictly common form (not GCD)
                        }
                    }
                } else if (e.getExponentiation().IsPrincipalSurd()) {
                    c = e.InCommonWith(*this);
                } else {
                    c = NaN();
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
        } else if (v.IsPrincipalSurd()) {
            auto commonWithBase = v.InCommonWith(ebase());
            if (commonWithBase != constants::one) {
                LOG_AND_IMPLEMENT(*this << " InCommonWith " << v);
            }
        } else {
            LOG_AND_IMPLEMENT(*this << " InCommonWith " << v);
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
            return NaN();
        }
    }

    Valuable::solutions_t Exponentiation::Distinct() const
    {
        solutions_t branches;
        if (eexp().IsSimpleFraction()){
            auto& f = eexp().as<Fraction>();
            auto& denom = f.denominator();
            if (denom.IsZero() || IsNaN()) {
                return { NaN() };
            }
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
                            branches.emplace(-branch * constants::i);
                            branches.emplace(branch * constants::i);
                            branches.emplace(-branch);
                            branches.emplace(std::move(const_cast<decltype(distinct)::reference>(branch)));
                        }
                    else
                    {
                        auto Branch = [](auto&& collection) {
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
							LOG_AND_IMPLEMENT("Distinct for " << *this);
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
        return IsVaExp()
            && eexp().IsInt()
            && eexp() > constants::zero;
    }

    Valuable& Exponentiation::solve(const Variable& va, solutions_t& s) const {
        if (_1 == va && !_2.FindVa() && _2 != constants::zero) {
            s.emplace(constants::zero);
            s.emplace(-constants::zero);
        } else {
            s.emplace(NaN());
        }
        return const_cast<Valuable&>(static_cast<const Valuable&>(*this));
    }

    Valuable Exponentiation::Sign() const {
        auto sign = ebase().Sign() ^ eexp();
        sign.optimize();
        return sign;
    }

    const PrincipalSurd* Exponentiation::PrincipalSurdFactor() const {
        return ebase().PrincipalSurdFactor();
    }
