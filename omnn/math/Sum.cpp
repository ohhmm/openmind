//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Sum.h"
#include "Euler.h"
#include "Formula.h"
#include "Fraction.h"
#include "i.h"
#include "Infinity.h"
#include "Integer.h"
#include "Modulo.h"
#include "Product.h"
#include "PrincipalSurd.h"
#include "pi.h"
#include "System.h"
#include "VarHost.h"
#include "Cache.h"

#include <omnn/rt/antiloop.hpp>
#include <rt/cl.h>
#include <rt/each.hpp>

//TODO:
//import std;
#include <algorithm>
#include <cmath>
#include <functional>
#include <future>
#include <map>
#include <numeric>
#include <stack>
#include <thread>
#include <type_traits>


namespace omnn::math {

namespace
{
    CACHE(DbSumBalancingCache);
    CACHE(DbSumGCDCache);
    CACHE(DbSumModCache);
    CACHE(DbSumOptimizationCache);
    CACHE(DbSumSolutionsOptimizedCache);
    CACHE(DbSumSolutionsAllRootsCache);
    CACHE(DbSumSolutionsARootCache);
    CACHE(DbSumSqCache);


        // inequality should cover all cases
        auto toc = [](const Valuable& x, const Valuable& y) // type order comparator
        {
			using namespace std;
            static type_index order[] = {
                typeid(NaN),
                typeid(MInfinity),
                typeid(Sum),
                typeid(Product),
                typeid(Exponentiation),
                typeid(Variable),
                typeid(Euler),
                typeid(Pi),
                typeid(MinusOneSurd),
                typeid(Integer),
                typeid(Fraction),
                typeid(PrincipalSurd),
                typeid(Modulo),
                typeid(Infinity),
            };

            static auto ob = std::begin(order);
            static auto oe = std::end(order);

            auto it1 = std::find(ob, oe, x.Type());
            assert(it1!=oe); // IMPLEMENT
            auto it2 = std::find(ob, oe, y.Type());
            assert(it2!=oe); // IMPLEMENT
            return it1 == it2 ? x > y : it1 < it2;
        };

        constexpr SumOrderComparator soc;
    }

    // store order operator
    bool SumOrderComparator::operator()(const Valuable& v1, const Valuable& v2) const
    {
        if (v1 == v2) {
            return false;  // Equal elements are never ordered before each other
        }

        // If types are different, use type ordering
        if (v1.Type() != v2.Type()) {
            if (v1.IsSum()) {
                auto &sum1 = v1.as<Sum>();
                if (sum1.size() == 1) {
                    return operator()(sum1.begin()->get(), v2);
                }
            }

            if (v2.IsSum()) {
                auto &sum2 = v2.as<Sum>();
                if (sum2.size() == 1) {
                    return operator()(v1, sum2.begin()->get());
                }
            }

            if (v1.IsProduct()) {
                auto &prod1 = v1.as<Product>();
                if (prod1.size() == 1) {
                    return operator()(prod1.begin()->get(), v2);
                }
            }

            if (v2.IsProduct()) {
                auto &prod2 = v2.as<Product>();
                if (prod2.size() == 1) {
                    return operator()(v1, prod2.begin()->get());
                }
            }

            if (!((v1.IsProduct() && v2.IsExponentiation())
                  || (v2.IsProduct() && v1.IsExponentiation()))
                    ) {
                return toc(v1, v2);
            }
        }

        // For same types, delegate to IsComesBefore
        // This maintains antisymmetry since IsComesBefore is designed for same-type comparison
        if (v1.IsComesBefore(v2)) {
            return true;
        }
        if (v2.IsComesBefore(v1)) {
            return false;
        }

        // If neither comes before the other and they're not equal, MSVC considering it as inconsistent ordering
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
        LOG_AND_IMPLEMENT("FIXME: inconsistent ordering, SumOrderComparator failed for not equal values: " << v1 << " and " << v2);
#endif
        return {};
    }

    Sum::iterator Sum::Had(iterator it)
    {
        auto item = *it;
        std::cout << item.str();
        it = std::find(members.begin(), members.end(), item);
        LOG_AND_IMPLEMENT("Impossible! Check order comparator error: " << item << " was in  " << *this);
        return it;
    }

    bool Sum::VarSurdFactor(const Valuable& v) {
        return (v.IsProduct() && v.as<Product>().HasSurdFactor())
            || (v.IsSum() && v.as<Sum>().HasSurdFactor())
            || (v.IsPrincipalSurd() && v.FindVa() != nullptr);
    }

    auto HwC = std::thread::hardware_concurrency();
    auto Thr = ::std::min<decltype(HwC)>(HwC << 3, 128);

    const Sum::iterator Sum::Add(const Valuable& item, const iterator hint) {
        auto copy = item;
        return this->Add(std::move(copy), hint);
    }

    const Sum::iterator Sum::Add(Valuable&& item, const iterator hint)
    {
        iterator it = hint;
        if (item.IsZero())
            it = end();
        else if (item.IsInt()) {
            it = GetFirstOccurence<Integer>();
            auto e = end();
            if (it == e) 
                it = GetFirstOccurence<Fraction>();
            if (it != e && it->IsSimple()) {
                auto i = Extract(it++);
                OptimizeOn oo;
                i += item;
                if (!i.IsZero())
                    it = Add(std::move(i), it);
                else
                    it = e;
            }
            else
                it = base::Add(std::move(item), hint);
        } else if (item.IsSimpleFraction()
            && (it = GetFirstOccurence<Fraction>()) != end()
            && it->IsSimple())
        {
            auto i = Extract(it++);
            OptimizeOn oo;
            i += item;
            if (!i.IsZero())
                it = Add(std::move(i), it);
            else
                it = end();
        } else if (item.IsSum()) {
            for(auto& i : item.as<Sum>()) {
                it = Add(i, it);
            }
        }
        else
        {
#if __has_include(<execution>)
            if (members.size() > Thr)
                it = std::find(
                               PAR
                               members.begin(), members.end(), item);
            else
#endif
                it = members.find(item);

            auto itemMaxVaExp = item.getMaxVaExp();
            if (it == end()) {
                it = base::Add(std::move(item), hint);
            } else {
                Update(it, std::move(item.shl()));
            }

            if(itemMaxVaExp > maxVaExp)
                maxVaExp = itemMaxVaExp;

            // FIXME: handle lowering of maxVaExp
        }
        return it;
    }
    
    void Sum::Update(iterator& it, Valuable&& value)
    {
        if (value.IsZero()) {
            Delete(it);
        } else {
            base::Update(it, std::move(value));
        }
    }

    void Sum::Update(iterator& it, const Valuable& value)
    {
        if (value.IsZero()) {
            Delete(it);
        } else {
            base::Update(it, value);
        }
    }

	Valuable Sum::operator -() const
	{
		Sum s;
		for (auto& a : members) 
			s.Add(-a);
		return s;
	}

    Valuable Sum::LCMofMemberFractionDenominators() const {
        auto lcm = constants::one;
        for (auto& m : members) {
            if (m.IsFraction()) {
                auto& f = m.as<Fraction>();
                auto& dn = f.getDenominator();
                if (dn != constants::one) {
                    lcm.lcm(dn);
                }
            } else if (m.IsExponentiation()) {
                auto& e = m.as<Exponentiation>();
                auto& ee = e.getExponentiation();
                if (ee == constants::minus_1) {
                    lcm.lcm(e.getBase());
                }
            } else if (m.IsProduct()) {
                auto& p = m.as<Product>();
                for (auto& m : p) {
                    if (m.IsFraction()) {
						auto& f = m.as<Fraction>();
						auto& dn = f.getDenominator();
                        if (dn != constants::one) {
							lcm.lcm(dn);
						}
                    }
                    else if (m.IsExponentiation()) {
						auto& e = m.as<Exponentiation>();
						auto& ee = e.getExponentiation();
                        if (ee == constants::minus_1) {
							lcm.lcm(e.getBase());
						}
					}
				}
            }
        }
        return lcm;
    }

    Valuable Sum::GCDofMembers() const {
        if (!is_optimized() && !isOptimizing) {
            OptimizeOn on;
            Valuable copy(Clone());
            copy.optimize();
            if (copy.is_optimized()) {
                return copy.IsSum()
                    ? copy.as<Sum>().GCDofMembers()
                    : std::move(copy);
            }
        }
        auto b = members.begin();
        auto e = members.end();
        auto it = b;
        auto gcd = it == e // empty sum
            ? constants::zero
            : size() == 1 // single element
            ? *it
            : b->GCD(*++it);
        if (size() > 1) {
            for (; gcd != constants::one && it != e; ++it) {
                if (it->IsZero()) {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
                    LOG_AND_IMPLEMENT("FIXME: GCD of members of non-optimized sums causes lesser GCD values: " << *this);
#endif
                    continue;
                }
                bool processed = false;
                if (it->IsPrincipalSurd()) {
                    auto& surd = it->as<PrincipalSurd>();
                    auto& subexpr = surd.Radicand();
                    if (subexpr.IsSum()) {
                        auto subgcd = subexpr.as<Sum>().GCDofMembers();
                        Valuable copy = surd;
                        copy.as<PrincipalSurd>().setRadicand(std::move(subgcd));
                        copy.optimize();
                        processed = !copy.FindVa();
                        if (processed) {
                            if (copy.IsInt())
                                gcd = boost::integer::gcd(gcd, copy);
                            else
                                gcd = copy.GCD(gcd);
                        }
                    }
                }
                if (!processed) {
                    gcd.gcd(it->varless());
                }
            }
        }
        return gcd;
    }

    Valuable Sum::GCDofCoefficients(const Variable& va) const {
        auto coefficients = Coefficients(va);
        auto beg = std::begin(coefficients);
        auto& init = coefficients.size() > 0 ? *beg++ : constants::zero;
        return std::reduce(beg, coefficients.end(), init,
            [](auto _1st, auto _2nd) { return _1st.GCD(_2nd); });
    }

    Valuable Sum::GCD(const Valuable& v) const {
        if (v.IsInt())
            return v.GCD(GCDofMembers()); 
        auto gcd = base::GCD(v);
        if (gcd == constants::one)
            gcd = v.GCD(GCDofMembers());
        return gcd;
    }

    bool Sum::IsZero() const {
        auto n = members.size();
        return n == 0
			|| (n == 1 && members.begin()->IsZero());
    }

    bool Sum::operator==(const Sum& v) const {
        return members == v.members
                || (members.size() == 1 && members.begin()->operator==(v))
                || (IsZero() && v.IsZero());
    }
    bool Sum::operator ==(const Valuable &v) const
    {
        return (Valuable::hash == v.Hash()
                && v.IsSum()
                && operator==(v.as<Sum>())
                )
                || (members.size() == 1 && members.begin()->operator==(v))
                || (IsZero() && v.IsZero());
    }
    
    void Sum::optimize()
    {
        if (is_optimized() || !optimizations)
            return;

        if (isOptimizing)
            return;

        ANTILOOP(Sum)

        Optimizing o(*this);

        auto s = str();
        auto doCheckCache = s.length() > 10;
        auto isBalancing = IsEquation();
        auto& db = isBalancing ? DbSumBalancingCache : DbSumOptimizationCache;
        auto cached = doCheckCache ? db.AsyncFetch(*this, true) : Cache::TaskNoCache;

        Valuable w;
        do
        {
            CHECK_OPTIMIZATION_CACHE

            w = *this;

            CHECK_OPTIMIZATION_CACHE

            if (members.size() == 0) {
                Become(0_v);
                return;
            }
            if (members.size() == 1) {
                Become(Extract(members.begin()));
                return;
            }

            CHECK_OPTIMIZATION_CACHE

            // Remove inner sums first, to eliminate recursive optimization loop
            for (auto it = members.begin(); it != members.end();) {
                if (it->IsSum()) {
                    for (auto& m : it->as<Sum>()) {
                        Add(std::move(m));
                    }
                    Delete(it);
                    continue;
                }
                else if (it->IsZero()) {
					Delete(it);
					continue;
				} else
					++it;
            }

            //if (isBalancing)
            //    balance();

            for (auto it = members.begin(); it != members.end();)
            {
                // optimize member
                auto copy = it->Optimized();
                if (!it->Same(copy))
                    Update(it, copy);
                else
                    ++it;

                CHECK_OPTIMIZATION_CACHE
            }

            if (IsEquation()) {
                auto e = members.end();
                auto Surd = [](auto it) {
                    auto surd = it->template As<PrincipalSurd>();
                    if (it->IsProduct()) {
                        auto& product = it->template as<Product>();
                        auto surdIt = product.template GetFirstOccurence<PrincipalSurd>();
                        if (surdIt != product.end()) {
                            surd = surdIt->template As<PrincipalSurd>();
                        }
                    }
                    return surd;
                };
                for (auto it = members.begin(); it != e;) {
                    auto SurdIsReducable = [&](auto& it) {
                        auto is = size() == 1; // sqrt(x)=0 roots is no differ from x=0 roots, but sqrt(x)+1=0 roots adter square (x=1) is different from x+1=0 (x=-1)
                                    // this means that only equality to zero which has zero sign can be squared (not for odd powers)
                                    // reducing surd makes new consistent equation which roots might be considered for the source equation but it is not equivalent
                                    // TODO: if size > 1, where expression under surd is not zero, it may be used for root finding routine, not for equation transformation

                        if (!is) {
                            auto isThereSurd = Surd(it);
                            if (isThereSurd) {
                                auto& index = isThereSurd->Index();
                                if (index.IsEven() == YesNoMaybe::No) {
                                    auto next = it;
                                    ++next;
                                    is = std::none_of(next, e,
                                            [this](auto& m) { return VarSurdFactor(m); }
                                        );
                                }
                            }
                        }
                        return is;
					};
                    if (it->IsPrincipalSurd()) {
                        if (SurdIsReducable(it)) {
                            auto ps = Extract(it);
                            auto& surd = ps.as<PrincipalSurd>();
                            operator^=(surd.Index());
                            operator-=(surd.Radicand());
                            break;
                        }
                    }
                    else if(it->IsProduct()) {
                        auto isThereSurd = Surd(it);
                        if (isThereSurd) {
                            if (SurdIsReducable(it)) {
                                auto& idx = isThereSurd->Index();
                                auto product = Extract(it);
                                operator^=(idx);
                                product.operator^=(idx);
                                operator-=(product);
                                break;
                            }
                            else {
                                ++it;
							}
                        } else {
							break;
						}
                    }
                    else
                        ++it;

                    CHECK_OPTIMIZATION_CACHE
                }

                if (IsSum()) {
                    auto common = GCDofMembers().varless();
                    if (common != constants::one) {
                        operator/=(common);
                    }
                }
            }
            
            CHECK_OPTIMIZATION_CACHE
            
            if (!IsSum()) {
                break;
            }

            for (auto it = members.begin(); it != members.end();)
            {
                if (it->IsSum()) {
                    for (auto& m : it->as<Sum>())
                    {
                        Add(std::move(m));
                    }
                    Delete(it);
                    continue;
                }
                
                auto it2 = it;
                ++it2;
                Valuable c = *it;
                Valuable mc, inc;
                
                auto up = [&](){
                    mc = -c;
                };

                up();
                
                auto comVaEq = [&]() {
                    auto& ccv = c.getCommonVars();
                    auto ccvsz = ccv.size();
                    auto& itcv = it2->getCommonVars();
                    auto itcvsz = itcv.size();
                    return ccvsz
                        && ccvsz == itcvsz 
                        && std::equal(//TODO:std::execution::par,
                            ccv.cbegin(), ccv.cend(), itcv.cbegin());
                };
                
                for (; it2 != members.end();)
                {
                    CHECK_OPTIMIZATION_CACHE
                    
                    if(c.IsSum()){
                        break;
                    }
                    auto simplified = it2->IsSummationSimplifiable(c);
                    if (simplified.first) {
                        c = std::move(simplified.second);
                        Delete(it2);
                        up();
                    }
                    else
                    if (((c.IsFraction() || c.IsInt()) && it2->IsSimpleFraction())
                        || (it2->IsInt() && (c.IsInt() || c.IsSimpleFraction()))
                        || (c.IsProduct() && mc == *it2)
                        || c.IsInfinity() || c.IsMInfinity()
                        || it2->IsInfinity() || it2->IsMInfinity()
                        )
                    {
                        c += *it2;
                        Delete(it2);
                        up();
                    }
                    else if(it2->Same(c))
                    {
                        c *= 2;
                        Delete(it2);
                        up();
                    }
                    else if(it2->Same(mc))
                    {
                        c = constants::zero;
                        Delete(it2);
                        up();
                    }
                    else if ((inc = it2->InCommonWith(c)) != constants::one
                             && inc.IsMultival() != YesNoMaybe::Yes) {
                            auto sum = (c / inc).IsSummationSimplifiable(*it2 / inc);
                            if(sum.first)
                            {
                                sum.second *= inc;
                                c = sum.second;
                                Delete(it2);
                                up();
                            } else
                                ++it2;
                    }
                    else if (c.IsFraction() && it2->IsFraction()
                             && c.as<Fraction>().getDenominator() == it2->as<Fraction>().getDenominator())
                    {
                        c += *it2;
                        Delete(it2);
                        up();
                    }
                    else
                        ++it2;
                }

                if (c==0)
                    Delete(it);
                else if (it->Same(c))
                    ++it;
                else
                    Update(it, c);
            }

            // optimize members
            for (auto it = members.begin(); it != members.end();)
            {
                CHECK_OPTIMIZATION_CACHE

                auto copy = it->Optimized();
                
                CHECK_OPTIMIZATION_CACHE
                
                if (!it->Same(copy)) {
                    Update(it, copy);
                }
                else
                    ++it;
            }
            
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
//            if (w!=*this) {
//                std::cout << "Sum optimized from \n\t" << w << "\n \t to " << *this << std::endl;
//            }
#endif
        } while (w != *this);

        if (IsSum()) {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
            if (size() == 1) {
                LOG_AND_IMPLEMENT("Sum has single member after being optimized from " << s << " to " << *this);
            }
#endif
            if (isBalancing)
                balance();
        }

        if (doCheckCache && cached.NotInCache()) {
            db.AsyncSet(std::move(s), str());
        }
    }

    void Sum::balance()
    {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
        std::cout << "Balancing " << *this << std::endl;
#endif
        if(IsSum())
        {
            if (members.size() == 0) {
                Become(0_v);
            }
            else if (members.size() == 1) {
                cont::iterator b = members.begin();
                Become(std::move(const_cast<Valuable&>(*b)));
            }
            else {
                // make coefficients int to use https://simple.wikipedia.org/wiki/Rational_root_theorem
                bool scan;
                do {
                    scan = {};
                    if (IsSum())
                    {
                        for (auto& m : members)
                        {
                            if (m.IsProduct()) {
                                auto& p = m.as<Product>();
                                for (auto& m : p) {
                                    if (m.IsFraction()) {
                                        operator*=(m.as<Fraction>().getDenominator());
                                        scan = true;
                                        break;
                                    }
                                    else if (m.IsExponentiation()) {
                                        auto& e = m.as<Exponentiation>();
                                        auto& ee = e.getExponentiation();
                                        if (ee.IsInt() && ee.ca() < 0) {
                                            operator*=(e.getBase() ^ (-ee)); // FIXME: (-1*((-1*percentWaterDehydrated + 100)^(-1))*potatoKgDehydrated + (percentWaterDehydrated^(-1))*weightWaterDehydrated)  *  (-1*percentWaterDehydrated + 100)

                                            scan = true;
                                            break;
                                        }
                                        else if (ee.IsFraction()) {
                                            auto& f = ee.as<Fraction>();
                                            auto& d = f.getDenominator();
                                            auto wo = p / e;
                                            Become(((e.getBase() ^ f.getNumerator()) * (wo^d)) - ((-(*this - p)) ^ d));
                                            scan = true;
                                            break;
                                        }
                                    }
                                }
                                if (scan)
                                    break;
                            }
                            else if (m.IsFraction()) {
                                operator*=(m.as<Fraction>().getDenominator());
                                scan = true;
                                break;
                            }
                            else if (m.IsExponentiation()) {
                                auto& e = m.as<Exponentiation>();
                                auto& ee = e.getExponentiation();
                                if (ee.IsInt() && ee < 0) {
                                    operator*=(e.getBase() ^ (-ee));
                                    scan = true;
                                    break;
                                }
                                else if (ee.IsFraction()) {
                                    auto& f = ee.as<Fraction>();
                                    Become((e.getBase() ^ f.getNumerator()) - ((-(*this - m)) ^ f.getDenominator()));
                                    scan = true;
                                    break;
                                }
                            }
                        }
                    }
                } while (scan);

                if (IsSum()) {
                    auto& coVa = getCommonVars();
                    if (coVa.size()) {
                        *this /= VaVal(coVa); // TODO : Add test: zero root disappeared
                    }

                    if(IsSum())
                    {
                        auto gcd = GCDofMembers();
                        if(gcd != constants::one){
                            operator/=(gcd);
                        }

                        if(IsMultival()== Valuable::YesNoMaybe::Yes){
                            auto uni = Univariate();
                            if(!Same(uni)){
                                Become(std::move(uni));
                            }
                        }
                    }
                }
            }
        }
    }

    const Valuable::vars_cont_t& Sum::getCommonVars() const
    {
        vars.clear();
        
        auto it = members.begin();
        if (it != members.end())
        {
            vars = it->getCommonVars();
            ++it;
            while (it != members.end()) {
                auto& _ = it->getCommonVars();
                for(auto i = vars.begin(); i != vars.end();)
                {
                    auto found = _.find(i->first);
                    if(found != _.end())
                    {
                        auto mm = std::minmax(i->second, found->second,
                                              [](auto& _1, auto& _2) { return std::abs(_1) < std::abs(_2); });
                        if ((i->second < 0) == (found->second < 0)) {
                            i->second = mm.first;
                        }
                        else
                        {
                            i->second = mm.second.Sign() * (std::abs(mm.second) - std::abs(mm.first));
                        }
                        ++i;
                    }
                    else
                    {
                        vars.erase(i++);
                    }
                }
                ++it;
            }
        }

        return vars;
    }
    
    Valuable Sum::InCommonWith(const Valuable& v) const
    {
        return GCD(v);
    }

    max_exp_t Sum::findMaxVaExp() {
        max_exp_t maxVaExp = 0;
		for (auto& m : members) {
			auto mMaxVaExp = m.getMaxVaExp();
			if (mMaxVaExp > maxVaExp)
				maxVaExp = mMaxVaExp;
		}
		return maxVaExp;
	}

    Valuable Sum::varless() const {
        return GCDofMembers().varless();
    }

    Valuable& Sum::operator+=(const Sum& sum) {
        if (sum.size()) {
            {
                OptimizeOff off;
                for (auto &item: sum) {
                    operator+=(item); // FIXME: operator+= -> Add
                }
            }
            optimize();
        }
		return *this;
	}

    Valuable& Sum::operator +=(const Valuable& v)
    {
        if (v.IsZero()) {
            return *this;
        }
        if (v.IsSum()) {
            operator+=(v.as<Sum>());
        }
        else
        {
            for (auto it = members.begin(); it != members.end(); ++it)
            {
                auto simplified = it->IsSummationSimplifiable(v);
                if (simplified.first) {
                    Update(it, std::move(simplified.second));
                    optimize();
                    return *this;
                }
                if (it->OfSameType(v)
                    && it->is_optimized() && v.is_optimized()
                    && it->getCommonVars() == v.getCommonVars())
                {
                    auto s = *it + v;
                    if (!s.IsSum()) {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
                        if (optimizations) {
                            std::cout << *it << " + " << v << " = " << s
                                      << "\t\tIMPLEMENT: must be covered by IsSummationSimpifiable call" << std::endl;
                            s = *it + v;
                            simplified = it->IsSummationSimplifiable(v);
                            // TODO: fix all these cases: LOG_AND_IMPLEMENT(*it << " + " << v << " = " << s << "\t\tmust
                            // be covered by IsSummationSimpifiable call");
                        }
#endif
                        Update(it, s);
                        optimize();
                        return *this;
                    }
                }
            }

            Add(v);
        }

        optimize();
        return *this;
    }

	Valuable& Sum::operator *=(const Valuable& v)
	{
        auto vIsInt = v.IsInt();
        if (vIsInt && v == 0)
        {
            Become(0);
            return *this;
        }
        else if (vIsInt && v == 1)
        {
            return *this;
        }
        else {
            Sum sum;
            {
                OptimizeOff off;
                sum.SetView(GetView());
                if (v.IsSum()) {
                    for (auto& _1 : v.as<Sum>())
                        for (auto& _2 : members)
                            sum.Add(_1 * _2);
                }
                else
                {
                    for (auto& a : members)
                    {
                        sum.Add(a * v);
                    }
                    if (vIsInt)
                        sum.optimized = optimized;
                }
            }
            Become(std::move(sum));
        }
        return *this;
	}

	Valuable& Sum::operator /=(const Valuable& v)
	{
        Valuable s = constants::zero;
        auto view = GetView();
        SetView(View::None);
		if (v.IsSum())
		{
            auto& i = v.as<Sum>();
            if(!v.FindVa())
            {
                for (auto& a : members) {
                    for (auto& b : i.members) {
                        s += a / b;
                    }
                }
            } else if (i.IsBinomial() && HasSameVars(i) && (!IsBinomial() && operator==(v))) {
                auto& im = i.members;
                auto root = -*im.rbegin();
                std::vector<Valuable> coefficients;
                auto& va = im.begin()->as<Variable>();
                auto degree = FillPolyCoeff(coefficients, va);
                if (degree) {
                    auto coef = std::move(coefficients.back());
                    coefficients.pop_back();
                    for (; degree-- > 0;) {
                        std::swap(coef, coefficients[degree]);
                        coef += coefficients[degree] * root;
                    }
                    if (coef == 0) {
                        auto powerX = 1_v;
                        for (auto& coef : coefficients) {
                            s += coef * powerX;
                            powerX *= va;
                        }
                    } else {
                        LOG_AND_IMPLEMENT(str() << " / " << v << " reminder " << coef);
                    }
                } else {
                    IMPLEMENT
                }
            }
            else
            {
                if (size() < i.size())
                {
                    s = Fraction(*this, v);
                }
                else if (HasSameVars(v))
                {
                    auto b = i.begin();
                    auto e = i.end();
                    size_t offs = 0;
                    std::deque<Valuable> hist {*this};
                    auto icnt = size() * 2;
                    while (!IsZero() && icnt--)
                    {
                        if(IsSum())
                        {
                            auto it = begin();
                            for (auto i=offs; it != end() && i--; ) {
                                ++it;
                            }
                            if (it == end()) {
                                IMPLEMENT;
                            }
                            auto vars = it->Vars();
                            auto it2 = b;
                            while(it2 != e && it2->Vars() != vars)
                                ++it2;
                            if (it2 == e) {
                                auto coVa = it->getCommonVars();
                                auto maxVa = std::max_element(coVa.begin(), coVa.end(),
                                                              [](auto&_1,auto&_2){return _1.second < _2.second;});
                                
                                for (it2 = b; it2 != e; ++it2)
                                {
                                    bool found = {};
                                    for(auto& v : it2->Vars())
                                    {
                                        found = v == maxVa->first;
                                        if (found) {
                                            auto coVa2 = it2->getCommonVars();
                                            auto coVa2vIt = coVa2.find(v);
                                            if (coVa2vIt == coVa2.end()) {
                                                IMPLEMENT
                                            }
                                            auto coVa1vIt = coVa.find(v);
                                            if (coVa1vIt == coVa.end()) {
                                                IMPLEMENT
                                            }
                                            found = coVa1vIt->second >= coVa2vIt->second;
                                            
                                        }
                                        
                                        if (!found) {
                                            break;
                                        }
                                    }
                                    
                                    if(found)
                                        break;
                                }
                                
                                if (it2 == e) {
                                    IMPLEMENT;
                                }
                            }

                            auto t = *begin() / *it2;
                            s += t;
                            t *= v;
                            *this -= t;
                            if (std::find(hist.begin(), hist.end(), *this) == hist.end()) {
                                hist.push_back(*this);
                                constexpr size_t MaxHistSize = 8;
                                if (hist.size() > MaxHistSize) {
                                    hist.pop_front();
                                    offs = 0;
                                }
                            }
                            else
                            {
                                ++offs;
                                hist.clear();
                            }
                        }
                        else
                        {
                            operator/=(v);
                            operator+=(s);
                            SetView(view);
                            optimize();
                            return *this;
                        }
                    }
                    if (!IsZero()) {
                        OptimizeOff off;
                        s = Fraction(*this, v);
                    }
                }
                else
                {
                    OptimizeOff off;
                    s = Fraction(*this, v);
                }
            }
		}
		else
		{
            OptimizeOff off;
            for (auto& a : members) {
                s += a / v;
            }
		}
        s.SetView(view);
        SetView(view);
        return Become(std::move(s));
	}

    Valuable& Sum::operator %=(const Valuable& value)
    {
        auto doCheckCache = size() > 20;
        USE_CACHE(DbSumModCache);

        Valuable sum = constants::zero;
        if (value.IsSum())
        {
            auto& i = value.as<Sum>();
            if(!value.FindVa())
            {
                for (auto& a : members) {
                    CHECK_CACHED_READY
                    for (auto& b : i.members) {
                        sum += a / b;
                    }
                }
            }
            else
            {
                CHECK_CACHED_READY
                if (size() < i.size())
                {
                    sum = Modulo(*this, value);
                }
                else if (HasSameVars(value))
                {
                    CHECK_CACHED_READY
                    auto b = i.begin();
                    auto e = i.end();
                    size_t offs = 0;
                    std::deque<Valuable> hist {*this};
                    
                    auto icnt = size() * 2;
                    while (!IsZero() && icnt--)
                    {
                        CHECK_CACHED_READY
                        if (IsSum())
                        {
                            auto it = begin();
                            for (auto i=offs; it != end() && i--; ) {
                                ++it;
                            }
                            if (it == end()) {
                                break;
                            }
                            auto vars = it->Vars();
                            auto it2 = b;
                            while(it2 != e && it2->Vars() != vars)
                                ++it2;
                            if (it2 == e) {
                                auto coVa = it->getCommonVars();
                                if (coVa.size()) {
                                    auto maxVa = std::max_element(coVa.begin(), coVa.end(), [](auto& _1, auto& _2) {
                                        return _1.second < _2.second;
                                    });

                                    for (it2 = b; it2 != e; ++it2) {
                                        bool found = {};
                                        for (auto& v : it2->Vars()) {
                                            found = v == maxVa->first;
                                            if (found) {
                                                auto coVa2 = it2->getCommonVars();
                                                auto coVa2vIt = coVa2.find(v);
                                                if (coVa2vIt == coVa2.end()) {
                                                    IMPLEMENT
                                                }
                                                auto coVa1vIt = coVa.find(v);
                                                if (coVa1vIt == coVa.end()) {
                                                    IMPLEMENT
                                                }
                                                found = coVa1vIt->second >= coVa2vIt->second;
                                            }

                                            if (!found) {
                                                break;
                                            }
                                        }

                                        if (found)
                                            break;
                                    }
                                }
                                if (it2 == e) {
                                    break;
                                }
                            }
                            
                            auto t = *begin() / *it2;
                            sum += t;
                            t *= value;
                            *this -= t;
                            if (std::find(hist.begin(), hist.end(), *this) == hist.end()) {
                                hist.push_back(*this);
                                constexpr size_t MaxHistSize = 8;
                                if (hist.size() > MaxHistSize) {
                                    hist.pop_front();
                                    offs = 0;
                                }
                            }
                            else
                            {
                                ++offs;
                                hist.clear();
                            }
                        }
                        else
                        {
                            sum = Modulo(*this, value);
                        }
                    }
                    if (!IsZero()) {
                        sum = Modulo(*this, value);
                    }
                }
                else
                {
                    sum = Modulo(*this, value);
                }
            }
        }
        else
        {
            for (auto& a : members) {
                sum += a % value;
            }
            sum = Modulo(sum,value);
        }

        CHECK_CACHED_READY
        Become(std::move(sum));

        STORE_TO_CACHE
        return *this;
    }

    Sum::operator double() const
    {
        double sum = 0;
        for(auto& i : members)
            sum += static_cast<double>(i);
        return sum;
    }

    Valuable& Sum::d(const Variable& x)
    {
        Valuable sum = constants::zero;
        auto add = [&](const Valuable& m)
        {
            if (sum.IsSum()) {
                sum.as<Sum>().Add(m);
            }
            else
                sum += m;
        };
        for(auto m : *this)
            add(m.d(x));
        
        return Become(std::move(sum));
    }

    Sum::Sum(std::initializer_list<Valuable> l)
    {
        for (const auto& arg : l)
        {
            if(arg.IsSum()) {
                auto& a = arg.as<Sum>();
                for(auto& m: a)
                    this->Add(m, end());
            }
            else
                this->Add(arg, end());
        }
    }

    bool Sum::IsComesBefore(const Valuable& v) const
    {
        if (v.IsSum()) {
            auto sz1 = size();
            auto& s = v.as<Sum>();
            auto sz2 = s.size();
            if (sz1 != sz2) {
                return sz1 > sz2;
            }

            auto beg = begin();
            auto sbeg = s.begin();
            for (auto i1 = beg, i2 = sbeg; i1 != end(); ++i1, ++i2) {
                if (*i1 != *i2) {
                    auto cmp12 = soc(*i1, *i2);
                    auto cmp21 = soc(*i2, *i1);
                    if (cmp12 == cmp21)
                        continue;
                    return cmp21;
                }
            }

            for (auto i1 = beg, i2 = sbeg; i1 != end(); ++i1, ++i2) {
                if (*i1 != *i2) {
                    return toc(*i1, *i2);
                }
            }
        }

        if (size() == 1) {
            return begin()->IsComesBefore(v);
        }

        return toc(*this, v);
    }

	std::ostream& Sum::print(std::ostream& out) const
	{
        std::stringstream s;
        s << '(';
        constexpr char sep[] = " + ";
		for (auto& b : members) 
            s << b << sep;
        auto str = s.str();
        auto cstr = const_cast<char*>(str.c_str());
        cstr[str.size() - sizeof(sep) + 1] = 0;
        out << cstr << ')';
		return out;
	}

    std::ostream& Sum::code(std::ostream& out) const
    {
        std::stringstream s;
        s << '(';
        constexpr char sep[] = " + ";
        for (auto& b : members)
            b.code(s) << sep;
        auto str = s.str();
        auto cstr = const_cast<char*>(str.c_str());
        cstr[str.size() - sizeof(sep) + 1] = 0;
        out << cstr << ')';
        return out;
    }

    // FIXME: It is wrong to deside on sum square root sign before evaluating variables due to their sign ambiguity
    Valuable Sum::Sqrt() const
    {
        if (!optimizations || !is_optimized()) {
            OptimizeOn o;
            Valuable copy = *this;
            copy.optimize();
            if (!copy.IsSum()) {
                return copy.Sqrt();
            }
        }

        if (IsEquation()) {
            auto vars = Vars();
            if (vars.size() == 1) {
                const auto& va = *FindVa();
                if (IsPolynomial(va)) {
                    std::vector<Valuable> coefficients;
                    auto grade = FillPolyCoeff(coefficients, va);
                    if (grade == 2) {
                        solutions_t solutions;
                        solve(va, solutions, coefficients, grade);
                        if (solutions.size() == 1) {
                            return va - solutions.begin()->get();
                        } else {
                            LOG_AND_IMPLEMENT("square root of polynomial " << get());
                        }
                    }
                }
            } else if (getMaxVaExp() == 2 && (size() == 3 || size() == 2)) {
                auto it = cbegin();
                auto a = it++->Sqrt();
                auto b = it->Sqrt();
                auto sum = a + b;
                if (operator==(sum.Sq())) {
                    return sum;
                } else {
                    // auto diff = a - b;
                    // if (operator==(diff.Sq())) {
                    //     return diff;
                    // }
                }
            }
        }

        // TODO: https://math.stackexchange.com/a/1854191/118612

        return PrincipalSurd(*this, 2);
    }

    Valuable& Sum::sq()
    {
        auto doCheckCache = Complexity() > 10;
        USE_CACHE(DbSumSqCache);
        Sum sum;
        auto e = end();
        for (auto i = begin(); i != e; ++i)
        {
            CHECK_CACHED_READY;
            sum.Add(i->Sq());
            for (auto j = i; ++j != e;)
            {
                sum.Add(*i * *j * 2);
            }
        }
        if (doCheckCache && cached.NotInCache())
        {
            auto key = str();
            OptimizeOn on;
            Become(std::move(sum));
            cache.AsyncSet(std::move(key), str());
        }
        else
            Become(std::move(sum));
        return *this;
    }

    Valuable Sum::calcFreeMember() const
    {
        Valuable _ = constants::zero;
        for(auto& m : *this) {
            _ += m.calcFreeMember();
        }
        return _;
    }

    Valuable::solutions_t Sum::Distinct() const {
        solutions_t branches = { constants::zero };
        for (auto& m : members) {
            solutions_t newBranches;
            for (auto&& branch : m.Distinct()) {
                for (auto& b : branches) {
                    newBranches.emplace(b + branch);
                }
            }
            branches = std::move(newBranches);
        }
        return branches;
    }

    bool Sum::IsBinomial() const {
        return members.size() == 2 
			&& members.begin()->IsVa()
			&& members.rbegin()->FindVa() == nullptr;
	}

    bool Sum::IsPolynomial(const Variable& v) const {
        auto isSum = !exp;
        auto is = isSum ? base::IsPolynomial(v) : exp->IsPolynomial(v);
        if (isSum && is) {
            auto exps = GetVaExps();
            auto grade = exps[v];
            is = grade.IsInt() && (grade < 5 || exps.size() == 1);
        }
        return is;
    }

    size_t Sum::FillPolyCoeff(std::vector<Valuable>& coefficients, const Variable& v) const
    {
        size_t grade = 0;
        OptimizeOn opt;
        if(IsMultival() == Valuable::YesNoMaybe::Yes){
            auto univariate = Univariate();
            if (!univariate.IsSum()) {
                IMPLEMENT
            } else {
                grade = univariate.as<Sum>().FillPolyCoeff(coefficients, v);
                return grade;
            }
        } else if(!IsPolynomial(v)){
            Valuable copy = *this;
            copy.SetView(View::Solving);
            copy.optimize(); // for Solving ^, object may morph, IsSum check and as<Sum> call required
            if (copy.IsPolynomial(v)) {
                grade = copy.as<Sum>().FillPolyCoeff(coefficients, v);
                return grade;
            } else {
                LOG_AND_IMPLEMENT("Need normalized polynomial of f(" << v << ") to get its coefficients: " << str());
            }
        }
        Sum c0;
        auto add = [&](auto i, Valuable&& a) {
            if (i) {
                if (coefficients.size() < i)
                    coefficients.resize(i);
                coefficients[i] += a;
            } else
                c0.Add(a);
        };
        // TODO : openmp
        //#pragma omp parallel default(none) shared(grade,coefficients)
        {
            OptimizeOff off;
        //#pragma omp for 
        for (auto& m : members)
        {
            if(!m.HasVa(v))
            {
                c0.Add(m);
            }
            else if (m.IsProduct())
            {
                auto& coVa = m.getCommonVars();
                auto it = coVa.find(v);
                auto noVa = it == coVa.end();
                if (noVa && m.HasVa(v))
                {
                    auto mco = m;
                    mco.optimize();
                    auto& coVa = m.getCommonVars();
                    it = coVa.find(v);
                    noVa = it == coVa.end();
                    if(noVa){
                        Valuable s = *this;
                        s.SetView(View::Solving);
                        s.optimize();
                        if (s.IsSum()) {
                            coefficients.clear();
                            return s.as<Sum>().FillPolyCoeff(coefficients, v);
                        } else {
                            IMPLEMENT
						}
                    }
                }

                auto vcnt = noVa ? constants::zero : it->second; // exponentation of va
                if (!vcnt.IsInt()) {
                    IMPLEMENT
                }
                int ie = static_cast<int>(vcnt);
                if (ie < 0)
                {
                    auto normalized = (v ^ -ie) * *this;
                    if(normalized.IsSum()){
                        coefficients.clear();
                        return normalized.as<Sum>().FillPolyCoeff(coefficients, v);
                    } else {
                        IMPLEMENT
                    }
                }
                else if (ie > grade) {
                    grade = ie;
                    if (ie >= coefficients.size()) {
                        coefficients.resize(ie + 1);
                    }
                }

                add(ie, m / (v^vcnt));
            }
            else if (m.IsVa() && m == v) {
                if(grade < 1) {
                    grade = 1;
                    if (coefficients.size() < grade+1)
                        coefficients.resize(grade+1);
                }
                ++coefficients[1];
            }
            else if(m.IsExponentiation())
            {
                auto& e = m.as<Exponentiation>();
                if (e.getBase() == v)
                {
                    auto& ee = e.getExponentiation();
                    if (ee.IsInt()) {
                        auto i = static_cast<decltype(grade)>(ee.ca());
                        if (i > grade) {
                            grade = i;
                            if (i >= coefficients.size()) {
                                coefficients.resize(i+1);
                            }
                        }
                        add(i, 1);
                    }
                    else
                        IMPLEMENT
                }

            }
            else {
                std::cout << m << std::endl;
                IMPLEMENT
            }
        }
        }
        if(c0.size()){
            OptimizeOn oo;
            if (coefficients.size())
                coefficients[0] = std::move(c0);
            else
                coefficients.emplace_back(std::move(c0));
        }

        while (coefficients.size() && coefficients.rbegin()->IsZero()) {
            coefficients.pop_back();
            if (grade)
                --grade;
        }

        if (grade > 2) {
            auto lcm = constants::one;
            OptimizeOn oo;
            for (auto& c : coefficients) {
                c.optimize();
                if (c.IsFraction()) {
                    auto& f = c.as<Fraction>();
                    auto& dn = f.getDenominator();
                    if (dn != constants::one) {
                        lcm.lcm(dn);
                    }
                }
            }
            if (lcm != constants::one) {
                for (auto& c : coefficients) {
					c *= lcm;
				}
			}
        }

        return grade;
    }
    
    std::vector<Valuable> Sum::Coefficients(const Variable& va) const {
        std::vector<Valuable> coefficients;
        auto grade = FillPolyCoeff(coefficients, va);
        return coefficients;
    }

    Valuable Sum::operator()(const Variable& va, const Valuable& augmentation) const
    {
        solutions_t solutions;
        {
        auto e = *this + augmentation;
        if (!e.IsSum()) {
            IMPLEMENT;
        }
        auto& es = e.as<Sum>();
        std::vector<Valuable> coefs;
        auto grade = es.FillPolyCoeff(coefs, va);
        if (coefs.size() && grade)
        {
            es.solve(va, solutions, coefs, grade);
#ifndef NDEBUG
            for (auto i=solutions.begin(); i != solutions.end();) {
                if (i->HasVa(va)) {
                    IMPLEMENT
                    solutions.erase(i++);
                }
                else
                    ++i;
            }
#endif
            
            if (solutions.size()) {
                return Valuable(std::move(solutions));
            }
        }
        }
        Valuable _ = augmentation;
        Valuable todo = constants::zero;
        for(auto& m : *this)
        {
            if (m.HasVa(va)) {
                todo += m;
            } else {
                _ -= m;
            }
        }
        
        if (todo.IsSum()) {
            auto coVa = todo.getCommonVars();
            auto it = coVa.find(va);
            if (it != coVa.end()) {
                todo /= it->first ^ it->second;
                if (todo.HasVa(va)){
                    if(_ == 0)
                        solutions.insert(0);
                    else
                        IMPLEMENT
                }
                if (it->second < 0) {
                    _ *= it->first ^ -it->second;
                    return _(va, todo);
                }
                else if (todo != constants::zero)
                {
                    Valuable solution = Fraction(_, todo);
                    if (it->second != constants::one) {
                        solution ^= it->second.Reciprocal();
                    }
                    else
                        solution.optimize();
                    solutions.insert(solution);
                }
            }
            else
            {
                auto& stodo = todo.as<Sum>();
                if (stodo.size() == 2)
                {
                    auto b = stodo.begin();
                    auto rb = stodo.rbegin();
                    if(b->IsFraction())
                    {
                        return (*b)(va, _ - *rb);
                    }
                    else if(rb->IsFraction())
                    {
                        return (*rb)(va, _ - *b);
                    }
                    else
                    {
                        auto solving = as<Sum>();
                        solving.SetView(View::Solving);
                        OptimizeOn on;
                        solving.optimize();
                        LOG_AND_IMPLEMENT("Solving: " << stodo << " = " << _);
                    }
                }
                else
                {
                    solutions.insert(constants::zero);
                    for(auto& m : stodo)
                    {IMPLEMENT
                        solutions_t incoming(std::move(solutions));
                        auto e = m(va, _);
                        IMPLEMENT // test it
                        for(auto& s : incoming)
                        {
                            solutions.insert(s + e);
                        }
                    }
                }
            }
        }
        else
        {
            return todo(va, _);
        }

        return Valuable(std::move(solutions));
    }
    
    bool Sum::IsPowerX(const std::vector<Valuable>& coefficients){
        auto coefIdx = coefficients.size() - 1;
        auto is = coefficients[coefIdx] != 0;
        for(; is && coefIdx --> 1; )
        {
            is = is && coefficients[coefIdx] == 0;
        }
        return is;
    }

    Valuable Sum::operator()(const Variable& va) const
    {
        Valuable::solutions_t s;
        if(IsMultival() == Valuable::YesNoMaybe::Yes){
            return Univariate()(va);
        }

        std::vector<Valuable> coefficients;
        auto grade = FillPolyCoeff(coefficients, va);

        if (coefficients.size() == grade && grade == 0) {
            throw std::runtime_error("Expression is empty");
        }
        if (coefficients.size() != grade+1){
#ifndef NDEBUG
            // grade = FillPolyCoeff(coefs, va);
#endif
            IMPLEMENT
        }
        if(grade==0){
#ifndef NDEBUG
            //coefficients.clear();
            //grade = FillPolyCoeff(coefficients, va);
#endif
            return Valuable(std::move(s));
        } else if (grade == 1) {
            return coefficients[0] / -coefficients[1];
        }

        if(IsPowerX(coefficients)){
            return (-(coefficients[0] / coefficients[grade])) ^ (constants::one / grade);
        }
        
        auto doCheckCache = grade > 2;
        auto checkCached = doCheckCache
                            ? DbSumSolutionsOptimizedCache.AsyncFetch(*this, true)
                            : Cache::TaskNoCache;
        if (grade == 2) {
            auto& a = coefficients[2];
            auto& b = coefficients[1];
            auto& c = coefficients[0];
            auto d = (b ^ 2) - 4_v * a * c;
            return ((d ^ constants::half) - b) / (a * 2);
        } else if (grade == 3) {
            //            Valuable de = *this;  de.d(va);
            //            auto& a = coefs[3];
            //            auto& b = coefs[2];
            //            auto& c = coefs[1];
            //            auto& d = coefs[0];
            //            auto f = a*va+b;  // (a(x^3)+b(x^2))/(x^2)
            //            auto s = c*va+d;  // cx+d
            //            auto t=s/f;
            //            auto fsgcd = //va.Sq()-t;
            //            auto x1_x2  = fac(va);
            //            auto x3 = va  - x1_x2;
            if(checkCached)
                return checkCached;
            return Valuable(Solutions(va)); // TODO : debug this and uncomment custom solution
        }
        else if (coefficients.size())// && grade && grade < 3)
        {
            if(checkCached)
                return checkCached;
            solve(va, s, coefficients, grade);
            for (auto i=s.begin(); i != s.end();) {
                if (i->HasVa(va)) {
                    IMPLEMENT
                    s.erase(i++);
                }
                else
                    ++i;
            }
        }

        if (!s.size())
        {
            auto testSolutions = [&](const auto& solutions){
                bool added = {};
                for(auto& so : solutions){
                    Valuable c = *this;
                    c.Eval(va, so);
                    c.optimize();
                    if (c==0) // valid solution
                    {
                        s.insert(so);
                        added = true;
                    }
                }
                return added;
            };

            if(coefficients[0].IsZero()) // no free coef -> zero root
            {
                if(testSolutions(solutions_t{0}))
                {
                    return 0_v;
                } else {
                    IMPLEMENT
                }
            }
            if(checkCached)
                return checkCached;

            // diffirrentials roots test
            std::stack<Valuable> diffs;
            {
            Valuable d = *this;
            for(auto g = grade; g --> 1 ;){
                d.d(va);
                diffs.push(d);
            }
            }
            // try diffs solutions
            auto tryCoupleTopDiffSolutions = [&](){
                bool added = {};
                for(int i=0; i<2; ++i){ // first two are simple to calculate
                    if(diffs.size()){
                        auto& d = diffs.top();
                        if(!d.IsInt() && testSolutions(d.Solutions(va)))
                            added = true;
                        diffs.pop();
                    }
                }
                return added;
            };
            if(checkCached)
                return checkCached;

            if(!tryCoupleTopDiffSolutions()) // check if some solutions found
            {
                // continue couple more grades
                if(tryCoupleTopDiffSolutions()
// TODO :                  || testSolutions(GetIntegerSolution(va)) // try integer root
                   )
                {
                    if(checkCached)
                        return checkCached;

                    Valuable c = *this;
                    for(auto& so: s) {
                        c /= va.Equals(so);
                        if(checkCached)
                            return checkCached;
                    }
                    for(auto& so: c.Solutions(va)){ // complete solving
                        s.insert(so);
                        if(checkCached)
                            return checkCached;
                    }
                    if(checkCached)
                        return checkCached;
                    return Valuable(std::move(s));
                }
                else
                {
                    // TODO : IMPLEMENT
                }
            }
            
            // TODO : IMPLEMENT, the next here needs debugging
            Valuable augmentation = constants::zero;
            Valuable _ = constants::zero;
            for(auto& m : *this)
            {
                if (m.HasVa(va)) {
                    _ += m;
                } else {
                    augmentation -= m;
                }
            }
            if(checkCached)
                return checkCached;

            return _(va, augmentation);
        }
        
        if(checkCached)
            return checkCached;
        
        Valuable pluralSolutionsExpression(std::move(s));
        if (checkCached.NotInCache())
            DbSumSolutionsOptimizedCache.AsyncSet(str(), pluralSolutionsExpression.str());
        return pluralSolutionsExpression;
    }
    
    Valuable::solutions_t Sum::GetIntegerSolution(const Variable& va) const
    {
        solutions_t solutions;
        Valuable singleIntegerRoot;
        bool haveMin = false;
        Valuable _ = *this;
        _.optimize();
        if (!_.IsSum()) {
            IMPLEMENT
        }

        auto& sum = _.as<Sum>();
        {
            std::vector<Valuable> coefficients;
            auto g = sum.FillPolyCoeff(coefficients, va);
            if (g < 3) {
                sum.solve(va, solutions, coefficients, g);

                if (solutions.size()) {
                    return solutions;
                }
            }
        }

        solutions_t zs;
        auto zz = sum.get_zeros_zones(va, zs);
        if(zs.size())
        {
            bool foundIntSolution = {};
            for(auto& s : zs)
            {
                foundIntSolution |= s.IsInt();
                solutions.insert(s);
            }
            if(foundIntSolution)
                return solutions;
            else
                IMPLEMENT
        }
        
        Valuable min;
        Valuable closest;
        auto finder = [&](const Integer* i) -> bool
        {
            auto c = _;
            if(!c.IsProduct())
                c.optimize();
            auto cdx = c;
            cdx.d(va);
            std::cout << "searching: f(" << va << ")=" << _ << "; f'=" << cdx << std::endl;
            return i->Factorization([&,c](const Valuable& i)
                                    {
                                        auto _ = c;
                                        _.Eval(va, i);
                                        _.optimize();
                                        
                                        auto found = _.IsZero();
                                        if (found) {
                                            std::cout << "found " << i << std::endl;
                                            singleIntegerRoot = i;
                                            solutions.insert(i);
                                        }
                                        else
                                        {
                                            auto d_ = cdx;
                                            d_.Eval(va, i);
                                            d_.optimize();
                                            std::cout << "trying " << i << " got " << _ << " f'(" << i << ")=" << d_ << std::endl;
                                            if(!haveMin || std::abs(_) < std::abs(min)) {
                                                closest = i;
                                                min = _;
                                                haveMin = true;
                                            }
                                        }
                                        return found;
                                    },
                                    Infinity(),
                                    zz);
        };
        
        auto freeMember = _.calcFreeMember();
        if(!freeMember.IsInt()) {
            IMPLEMENT
        }
        auto& i = freeMember.as<Integer>();
        
        if (finder(&i)) {
            return solutions;
        }
        
        return solutions;
        IMPLEMENT

    }
    
    void Sum::solve(const Variable& va, solutions_t& solutions) const
    {
        std::vector<Valuable> coefficients;
        auto grade = FillPolyCoeff(coefficients, va);
//        if(grade > 2)
//        {
//            Valuable t = *this;
//            auto intSol = GetIntegerSolution(va);
//            for(auto is : intSol)
//                if(Test(va,is))
//                {
//                    solutions.insert(is);
//                    t /= va.Equals(is);
//                }
//            if(intSol.size()) {
//                t.optimize();
//                t.solve(va,solutions);
//            } else {
//                solve(va, solutions, coefficients, grade);
//            }
//        }
//        else
            solve(va, solutions, coefficients, grade);
    }
    
    void Sum::solve(const Variable& va, solutions_t& solutions, const std::vector<Valuable>& coefficients, size_t grade) const
    {
        if(coefficients.size() == grade && grade == 0)
		{
			return;
		}
        if(coefficients.size() != grade + 1) {
            std::cout << "Solving grade " << grade << " equation: " << str() << std::endl;
            IMPLEMENT
        }
        switch (grade) {
            case 1: {
                //x=-(b/a)
                auto& b = coefficients[0];
                auto& a = coefficients[1];
                solutions.insert(-b / a);
                break;
            }
            case 2: {
                if(!enforce_solve_using_rational_root_test_only){
                    // square equation x=(-b+√(b*b-4*a*c))/(2*a)
                    auto& a = coefficients[2];
                    auto& b = coefficients[1];
                    auto& c = coefficients[0];
                    auto d = (b ^ 2) - 4_v * a * c;
                    auto a2 = a * 2;
                    if (d == 0) {
                        solutions.insert(-b / a2);
                    } else {
                        auto dsq = d.Sqrt();
                        solutions.insert((-dsq-b)/a2);
                        solutions.insert((dsq-b)/a2);
                    }
                    break;
                }
                [[fallthrough]];
            }
            default: {
                // RATIONAL ROOT TEST
                auto asyncCheckAllRootsCached = DbSumSolutionsAllRootsCache.AsyncFetchSet(*this);
                if(asyncCheckAllRootsCached){
                    solutions = asyncCheckAllRootsCached;
                    return;
                }
                auto asyncCheckAnyRootCached = DbSumSolutionsARootCache.AsyncFetch(*this, true);
                if(asyncCheckAllRootsCached){
                    solutions = asyncCheckAllRootsCached;
                    return;
                }
                auto SolveTheRest = [&](Valuable&& oneRoot){
                    if(asyncCheckAnyRootCached.NotInCache())
                        DbSumSolutionsARootCache.AsyncSet(str(), oneRoot.str());
                    auto restToSolve = *this / va.Equals(oneRoot);
                    if(asyncCheckAllRootsCached){
                        solutions = asyncCheckAllRootsCached;
                        return;
                    }
                    restToSolve.solve(va, solutions);
                    solutions.emplace(std::move(oneRoot));
                    if(asyncCheckAllRootsCached.NotInCache())
                        DbSumSolutionsAllRootsCache.AsyncSetSet(*this, solutions);
                };
                if (asyncCheckAnyRootCached) {
                    SolveTheRest(asyncCheckAnyRootCached);
                    return;
                }

                auto isNormalizedPolynomial = true;
                auto lcm = constants::one;
				for(auto& c : coefficients) {
                    if (!c.IsInt()) {
                        isNormalizedPolynomial = false;
                        if (c.IsFraction()) {
                            auto& f = c.as<Fraction>();
                            auto& dn = f.getDenominator();
                            if (dn != constants::one) {
                                lcm.lcm(dn);
                            }
                        } else {
                            LOG_AND_IMPLEMENT("Solving " << va << " in " << *this << std::endl
        								<< "need to normalize coefficient: " << c << std::endl);
                        }
                    }
				}

                if (lcm != constants::one) {
                    LOG_AND_IMPLEMENT("Coefficients meant to be balanced before solving");
				}

                if(GetView() != View::Solving && GetView() != View::Equation) {
//                    auto 
                }
                auto a = coefficients[grade];
                if(!a.IsInt()) {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
                    std::cout << "first member needed optimization: " << a << std::endl;
#endif
                    OptimizeOn oo;
                    a.optimize();
                }
                auto k = coefficients[0];
                if(!k.IsInt()) {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
                    std::cout << "free member needed optimization: " << k << std::endl;
#endif
                    OptimizeOn oo;
                    k.optimize();
                }
                if(a.IsInt() && k.IsInt()) {
                    Valuable test;
                    auto& ai = a.as<Integer>();
                    auto& ki = k.as<Integer>();
                    std::set<Valuable> kiFactors;
                    if(asyncCheckAllRootsCached){
                        solutions = asyncCheckAllRootsCached;
                        return;
                    }
                    auto found = ai.Factorization([&](const auto& i){
                            auto iz = i != 0;
                            auto checkValues = [&](const auto& ik) -> bool {
                                kiFactors.insert(ik);
                                test = ik / i;
                                return Test(va, test) || Test(va, test = -test);
                            };
                            if (iz) {
                                if (kiFactors.empty())
                                    iz = ki.Factorization(checkValues, Infinity());
                                else
                                    iz = std::any_of(kiFactors.begin(), kiFactors.end(), checkValues);
							}
                            return iz;
                        }, Infinity());
                    if(asyncCheckAllRootsCached){
                        solutions = asyncCheckAllRootsCached;
                        return;
                    }
                    if(found) {
                        SolveTheRest(std::move(test));
                        return;
                    }
                } else {
                    LOG_AND_IMPLEMENT("Solving " << va << " in " << *this << std::endl
                        << "free member: " << k << std::endl
                        << "first member coefficient: " << a << std::endl
                        << "IsPowerX: " << IsPowerX(coefficients) << std::endl);
                }
            }
        }
        if(solutions.size())
            return;
        
        // no rational roots
        
        switch (grade) {
            case 3: {
//                static const VarHost::ptr VH(VarHost::make<std::string>());
////                static const Variable X = VH->New(std::string("x"));
////                static const Variable y = VH->New(std::string("y"));
////                static const Variable lambda = VH->New(std::string("lambda"));
//                Variable X,y,lambda;
//                Valuable co = *this;
//                co.Eval(va,X+(1_v/2));
//                co.Eval(X,y+lambda/y);
//                co.optimize();
//                co *= y^3;
//
//
//

//                // https://www.wolframalpha.com/input/?i=ax3%2Bbx2%2Bcx%2Bk%3D0
//                static const Variable a = VH->New(std::string("a"));
//                static const Variable b = VH->New(std::string("b"));
//                static const Variable c = VH->New(std::string("c"));
//                static const Variable k = VH->New(std::string("k"));
//                //(sqrt((-27 a^2 k + 9 a b c - 2 b^3)^2 + 4 (3 a c - b^2)^3) - 27 a^2 k + 9 a b c - 2 b^3)^(1/3)/(3 2^(1/3) a) - (2^(1/3) (3 a c - b^2))/(3 a (sqrt((-27 a^2 k + 9 a b c - 2 b^3)^2 + 4 (3 a c - b^2)^3) - 27 a^2 k + 9 a b c - 2 b^3)^(1/3)) - b/(3 a)
//                static const Valuable
//                    m27a2k = a.Sq()*k*-27,
//                    b2 = b.Sq(),
//                    m2b3 = b2*b*-2,
//                    _3ac = a*c*3,
//                    _9abc = _3ac*b*3,
//                    m27a2k_9abc_m2b3 = m27a2k+_9abc+m2b3, // -27 a^2 k + 9 a b c - 2 b^3
//                    _3ac_mb2 = _3ac-b.Sq(),
//                    sqrt__sq_m27a2k_9abc_m2b3__4_3ac_mb2_3 = (m27a2k_9abc_m2b3.Sq() + _3ac_mb2.Sq()*_3ac_mb2*4)^(1_v/2),
//                    cubrt = (sqrt__sq_m27a2k_9abc_m2b3__4_3ac_mb2_3 + m27a2k_9abc_m2b3) ^ (1_v/3),
//                    cubrt2 = 2_v^(1_v/3),
//                    X3 = cubrt/(a*cubrt2*3) - cubrt2*_3ac_mb2/(a*3*cubrt) -b/(a*3);
//                Valuable x = X3;
//                auto o = optimizations;
//                optimizations = {};
//                x.Eval(a,coefficients[3]);
//                x.Eval(b,coefficients[2]);
//                x.Eval(c,coefficients[1]);
//                x.Eval(k,coefficients[0]);
//                optimizations = o;
//                x.optimize();
//                if(!Test(va,x))
//                {
//                    Variable x1,x2,x3;
//                    // Viets:
//                    // x3 = -k/(a*x1*x2)
//                    // x1*x2 + x1*x3 + x2*x3 = c/a
//
//                    IMPLEMENT
//                }
                
//                // https://en.wikipedia.org/wiki/Cubic_function#General_solution_to_the_cubic_equation_with_real_coefficients
//                auto& a = coefficients[3];
//                auto& b = coefficients[2];
//                auto& c = coefficients[1];
//                auto& d = coefficients[0];

//                auto _1 = -(va*c+d);
//                auto _2 = (va*a+b);
//                auto _ = _1 % _2;
//                auto m = Modulo::cast(_);
//                if(m)
//                {
//                    auto _a = (m->get1() - _1) / _2;
//
//                }
//                else
//                {
//                    IMPLEMENT
//                }
//                auto xsq = -(va*c+d)/(va*a+b);
//
//                auto _3a = a*3;
//                auto mbd3a = -b/_3a;
//                auto cd3a = c/_3a;
//                auto dd2a = d/(a*2);
//                auto mbd3a_sq = mbd3a.Sq();
//                auto mbd3a_cube = mbd3a_sq * mbd3a;
//                auto cd3a_m_mbd3a_sq = cd3a - mbd3a_sq;
//                auto cd3a_m_mbd3a_sq_cube = cd3a_m_mbd3a_sq ^ 3;
//                _ = mbd3a_cube /*- cd3a*mbd3a*3/2 - dd2a*/ + (b*c-a*3*d)/(a.Sq()*2);
//                _1 = (_.Sq() + cd3a_m_mbd3a_sq_cube).sqrt();
//                auto x = ((_+_1)^1/3) + ((_-_1)^1/3) + mbd3a;
//                if(Test(va,x)){
//                    (*this / va.Equals(x)).solve(va, solutions);
//                    solutions.insert(std::move(x));
//                    break;
//                }else{
//                    //IMPLEMENT
//
//                    // four grade equation ax^4+bx^3+cx^2+dx+e=0
//                    // see https://math.stackexchange.com/questions/785/is-there-a-general-formula-for-solving-4th-degree-equations-quartic
//                    auto z = 0_v;
//                    auto& a = z;
//                    auto& b = coefficients[3];
//                    auto& c = coefficients[2];
//                    auto& d = coefficients[1];
//                    auto& e = coefficients[0];
//                    auto sa = a*a;
//                    auto sb = b*b;
//                    auto p1 = 2*c*c*c-9*b*c*d+27*a*d*d+27*b*b*e-72*a*c*e;
//                    auto p2 = p1+(4*((c*c-3*b*d+12*a*e)^3)+(p1^2)).sqrt();
//                    auto qp2 = (p2/2)^(1_v/3);
//                    p1.optimize();
//                    p2.optimize();
//                    qp2.optimize();
//                    auto p3 = (c*c-3*b*d+12*a*e)/(3*a*qp2)+qp2/(3*a);
//                    auto p4 = (sb/(4*sa)-(2*c)/(3*a)+p3).sqrt();
//                    auto p5 = sb/(2*sa)-(4*c)/(4*a)-p3;
//                    auto p6 = (-sb*b/(sa*a)+4*b*c/sa-8*d/a)/(4*p4);
//                    auto xp1 = b/(-4*a);
//                    auto xp2 = p4/2;
//                    auto xp3_1 = (p5-p6).sqrt()/2;
//                    auto xp3_2 = (p5+p6).sqrt()/2;
//                    solutions.insert(xp1-xp2-xp3_1);
//                    solutions.insert(xp1-xp2+xp3_1);
//                    solutions.insert(xp1+xp2-xp3_2);
//                    solutions.insert(xp1+xp2+xp3_2);
//                    break;
//                }
//
//                //
                auto& a = coefficients[3]; // 2
                auto& b = coefficients[2]; // -3
                auto& c = coefficients[1]; // -13
                auto& d = coefficients[0]; // 19
                
                auto asq = a.Sq(); // 4
                auto bsq = b.Sq(); // 9
                auto ac3 = a * c * 3; // -78
                auto di = bsq*c.Sq() - 4_v*a*(c^3) - 4_v*(b^3)*d - 27_v*asq*d.Sq() + ac3*(18_v/3)*b*d; // 8837
                auto d0 = bsq - ac3; //87
                if (di.IsZero())
                {
                    if (d0.IsZero())
                    {
                        solutions.emplace(b / (a*-3));
                    }
                    else
                    {
                        solutions.emplace((a*d * 9 - b * c) / (d0 * 2));
                        solutions.emplace((a*b*c * 4 + asq * d*-9 - bsq * b) / (d0 * a));
                    }
                }
                else
                {
                    auto d1 = (bsq * 2 - ac3 * 3)*b + asq * d * 27; // +
                    auto subC = (asq*di*-27).Sqrt();
                    auto C = ((d1 + ((d1.Sq() - d0.Sq() * d0 * 4) ^ constants::half)) / 2) ^ (constants::one / 3); // +
                    auto _1 = (a * -3).reciprocal();                                                        // +
                    auto _2 = b+C+(d0/C); // +
                    auto x = _1*_2; // complex with real part that is root
                    if(Test(va,x)) // << error in Eval
                    {
                        (*this / va.Equals(x)).solve(va, solutions);
                        solutions.emplace(std::move(x));
                    }
                    else if(x.IsMultival() == Valuable::YesNoMaybe::Yes
                        && x.Vars().empty()
                        && !x.IsSimple()
                    ) {
                        solutions = x.Distinct(); //FIXME: TODO: Debug
                    }
                    else
                    {
                        IMPLEMENT
                    }
//                    auto C1 = ((d1 + subC) / 2) ^ (1_v / 3);
//                    auto C2 = ((d1 - subC) / 2) ^ (1_v / 3);
//                    auto x = (b + C1 + d0 / C1) / (a*-3);
//                    if(Test(va, x))
//                        solutions.insert(x);
//                    x = (b + C1 + d0 / C2) / (a*-3);
//                    if(Test(va, x))
//                        solutions.insert(x);
//                    x = (b + C2 + d0 / C1) / (a*-3);
//                    if(Test(va, x))
//                        solutions.insert(x);
//                    x = (b + C2 + d0 / C2) / (a*-3);
//                    if(Test(va, x))
//                        solutions.insert(x);
                }
                break;
            }
            case 4: { // TODO: convert to sqrt-less form
                // four grade equation ax^4+bx^3+cx^2+dx+e=0
                // see https://math.stackexchange.com/questions/785/is-there-a-general-formula-for-solving-4th-degree-equations-quartic
                auto& a = coefficients[4];
                auto& b = coefficients[3];
                auto& c = coefficients[2];
                auto& d = coefficients[1];
                auto& e = coefficients[0];

                // Detect biquadratic equation
                if (b == d && b.IsZero()) {
                    auto squareSolutions = solutions;
                    solve(va, squareSolutions, {e, c, a}, 2);
                    for (auto& s : squareSolutions) {
                        auto solution = s ^ constants::half;
                        auto sqrtSquareSolutions = solution.Distinct();
                        solutions.insert(
                            std::make_move_iterator(sqrtSquareSolutions.begin()),
                            std::make_move_iterator(sqrtSquareSolutions.end()));
                    }
                } else {
                    auto sa = a * a;
                    auto sb = b * b;
                    auto p1 = 2 * c * c * c - 9 * b * c * d + 27 * a * d * d + 27 * b * b * e - 72 * a * c * e;
                    auto p2 = p1 + (4 * ((c * c - 3 * b * d + 12 * a * e) ^ 3) + (p1 ^ 2)).Sqrt();
                    auto qp2 = (p2 / 2) ^ (constants::one / 3);
                    p1.optimize();
                    p2.optimize();
                    qp2.optimize();
                    auto p3 = (c * c - 3 * b * d + 12 * a * e) / (3 * a * qp2) + qp2 / (3 * a);
                    auto p4 = (sb / (4 * sa) - (2 * c) / (3 * a) + p3).Sqrt();
                    auto p5 = sb / (2 * sa) - (4 * c) / (4 * a) - p3;
                    auto p6 = (-sb * b / (sa * a) + 4 * b * c / sa - 8 * d / a) / (4 * p4);
                    auto xp1 = b / (-4 * a);
                    auto xp2 = p4 / 2;
                    auto xp3_1 = (p5 - p6).Sqrt() / 2;
                    auto xp3_2 = (p5 + p6).Sqrt() / 2;
                    solutions.emplace(xp1 - xp2 - xp3_1);
                    solutions.emplace(xp1 - xp2 + xp3_1);
                    solutions.emplace(xp1 + xp2 - xp3_2);
                    solutions.emplace(xp1 + xp2 + xp3_2);
                }
                break;
            }
            default: {
                // build OpenCL kernel
#ifdef OPENMIND_USE_OPENCL
				using namespace boost::compute;
                Valuable copy = *this;
                copy.optimize();
                std::stringstream source;
                source << "__kernel void f(__global long *c) {"
                        << "    const uint i = get_global_id(0);"
                        << "    long " << va << "=i;"
                        << "    c[i] = "; copy.code(source);
                source << ";}";
                
                auto devices = system::devices();
                if(devices.size() == 0)
                	return;
                auto& cuwinner = ::omnn::rt::GetComputeUnitsWinnerDevice();
                auto wgsz = cuwinner.max_work_group_size();
                context context(cuwinner);
                
                kernel k(program::build_with_source(source.str(), context), "f");
                auto sz = wgsz * sizeof(cl_long);
                buffer c(context, sz);
                k.set_arg(0, c);
                
                command_queue queue(context, cuwinner);
                // run the add kernel
                queue.enqueue_1d_range_kernel(k, 0, wgsz, 0);
                
                // transfer results to the host array 'c'
                std::vector<cl_long> z(wgsz);
                queue.enqueue_read_buffer(c, 0, sz, &z[0]);
                queue.finish();
                
                Valuable simple = *this;
                auto addSolution = [&](auto& s) -> bool {
                    auto it = solutions.insert(s);
                    if (it.second) {
                        simple /= va - s;
                    }
                    return it.second;
                };
                
                for(auto i = wgsz; i-->0;)
                    if (z[i] == 0) {
                        // lets recheck on host
                        Valuable copy = *this;
                        copy.Eval(va, i);
                        copy.optimize();
                        if (copy.IsZero()) {
                            addSolution(i);
                        }
                    }
                
                if(solutions.size() == grade)
                    return;
                
                auto simpleSolve = [&](){
                    solutions_t news;
                    do {
                        news.clear();
                        simple.solve(va, news);
                        for(auto& s : news)
                        {
                            addSolution(s);
                        }
                    } while(news.size());
                };
                
                auto addSolution2 = [&](auto& _) -> bool {
                    if (addSolution(_)) {
                        simpleSolve();
                    }
                    return solutions.size() == grade;
                };
                
                
#define IS(_) if(addSolution2(_))return;
                
                if (solutions.size()) {
                    simpleSolve();
                }
                
                if(solutions.size() == grade)
                    return;
                
                
                for(auto i : simple.GetIntegerSolution(va))
                {
                    IS(i);
                }
                
                // decomposition
                IMPLEMENT;
//                auto yx = -*this;
//                Variable y;
//                yx.Valuable::Eval(va, y);
//                yx += *this;
//                yx /= va - y;
//                auto _ = yx.str();
                
                sz = grade + 1;
                auto sza = (grade >> 1) + (grade % 2) + 1;
                auto szb = (grade >> 1) + 1;
                std::vector<Variable> vva(sza);
                std::vector<Variable> vvb(szb);
                Valuable eq1, eq2;
                for (auto i = sza; i--; ) {
                    eq1 += vva[i] * (va ^ i);
                }
                for (auto i = szb; i--; ) {
                    eq2 += vvb[i] * (va ^ i);
                }
                
                auto teq = eq1*eq2;
                std::vector<Valuable> teq_coefficients;
                if (teq.IsSum()) {
                    auto& teqs = teq.as<Sum>();
                    if (teqs.FillPolyCoeff(teq_coefficients, va) != grade) {
                        IMPLEMENT
                    }
                } else {
                    IMPLEMENT
                }
                System sequs;
                for (auto i = sz; i--; ) {
                    auto c = coefficients[i];
                    Valuable eq = teq_coefficients[i]-c;
                    sequs << eq;
                }
                //sequs << copy;
                std::vector<solutions_t> s(szb);
                for (auto i = szb; i--; ) {
                    s[i] = sequs.Solve(vvb[i]);
                }
                
                auto ss = sequs.Solve(vvb[0]);
                if (ss.size()) {
                    for(auto& s : ss)
                    {
                        copy /= va - s;
                    }
                    copy.solve(va, solutions);
                }
                else
                    IMPLEMENT;
                break;
#endif
            }
        }

#if !defined(NDEBUG) && !defined(NOOMDEBUG)
        for (auto& s : solutions) {
            if (!Test(va, s)) {
                LOG_AND_IMPLEMENT(s << " is not a root of " << *this);
            }
        }
#endif
    }
    
    Valuable Sum::SumOfRoots() const
    {   // See Viet's basic formulas: https://en.m.wikipedia.org/wiki/Vieta%27s_formulas#Basic_formulas
        auto vars = Vars();
        if (vars.size() != 1) {
                IMPLEMENT
        }
        auto& va = *vars.cbegin();
        std::vector<Valuable> coefficients;
        auto grade = FillPolyCoeff(coefficients, va);
        if (coefficients.size() < 2) {
                IMPLEMENT
        }
        auto it = coefficients.begin();
        auto& lastCoeff = *it;
        auto& preLastCoeff = *++it;
        auto sumOfRoots = -(preLastCoeff / lastCoeff);
        return sumOfRoots;
    }

    Valuable Sum::ProductOfRoots() const
    {
        // See Viet's basic formulas: https://en.m.wikipedia.org/wiki/Vieta%27s_formulas#Basic_formulas
        auto vars = Vars();
        if (vars.size() != 1) {
                IMPLEMENT
        }
        auto& va = *vars.cbegin();
        std::vector<Valuable> coefficients;
        auto grade = FillPolyCoeff(coefficients, va);
        if (coefficients.size() < 2) {
                IMPLEMENT
        }
        auto prod = coefficients[0] / *coefficients.rbegin();
        if (grade % 2) {
            prod *= -1;
        }
        return prod;
    }

    /** fast linear equation formula deduction */
	Formula Sum::FormulaOfVa(const Variable& v) const
	{
        Valuable fx(0);
        std::vector<Valuable> coefficients(4);
        auto grade = FillPolyCoeff(coefficients,v);
        
        switch (grade) {
            case 2: {
                // square equation axx+bx+c=0
                // root formula: x=((b*b-4*a*c)^(1/2)-b)/(2*a)
                auto& a = coefficients[2];
                auto& b = coefficients[1];
                auto& c = coefficients[0];
                fx = ((b*b-4*a*c).Sqrt()-b)/(2*a);
                break;
            }
            case 4: {
                // four grade equation ax^4+bx^3+cx^2+dx+e=0
                // see https://math.stackexchange.com/questions/785/is-there-a-general-formula-for-solving-4th-degree-equations-quartic
                auto& a = coefficients[4];
                auto& b = coefficients[3];
                auto& c = coefficients[2];
                auto& d = coefficients[1];
                auto& e = coefficients[0];
                auto sa = a*a;
                auto sb = b*b;
                auto p1 = 2*c*c*c-9*b*c*d+27*a*d*d+27*b*b*e-72*a*c*e;
                auto p2 = p1+(4*((c*c-3*b*d+12*a*e)^3)+(p1^2)).Sqrt();
                auto qp2 = (p2/2)^(constants::one/3);
                p1.optimize();
                p2.optimize();
                qp2.optimize();
                auto p3 = (c*c-3*b*d+12*a*e)/(3*a*qp2)+qp2/(3*a);
                auto p4 = (sb/(4*sa)-(2*c)/(3*a)+p3).Sqrt();
                auto p5 = sb/(2*sa)-(4*c)/(4*a)-p3;
                auto p6 = (-sb*b/(sa*a)+4*b*c/sa-8*d/a)/(4*p4);
                fx = -b/(4*a)+p4/2+(p5+p6).Sqrt()/2;
//                fx.optimize();
                break;
            }
            default: {
                IMPLEMENT
                break;
            }
        }
        
        return Formula::DeclareFormula(v, fx);
	}

    bool Sum::SumIfSimplifiable(const Valuable& v){
        auto is = IsSummationSimplifiable(v);
        if(is.first)
            Become(std::move(is.second));
        return is.first;
    }

	bool Sum::MultiplyIfSimplifiable(const Valuable& v) {
        auto is = IsMultiplicationSimplifiable(v);
        if (is.first)
            Become(std::move(is.second));
        return is.first;
    }

    std::pair<bool,Valuable> Sum::IsSummationSimplifiable(const Valuable& v) const{
        std::pair<bool, Valuable> is{{}, v + *this};
        is.first = is.second.Complexity() < Complexity() + v.Complexity();
		return is;
    }

    std::pair<bool, Valuable> Sum::IsMultiplicationSimplifiable(const Valuable& v) const {
        std::pair<bool, Valuable> is;
        Sum sum{};
        for (auto& m : members) {
            auto mIs = v.IsMultiplicationSimplifiable(m);
            if (mIs.first) {
                is.first = mIs.first;
                sum.Add(mIs.second);
            } else {
                OptimizeOff off;
                sum.Add(Valuable(std::make_shared<Product>(std::initializer_list<Valuable>{v,m})));
            }
        }
        if (is.first) {
            is.second = sum;
            is.second.optimize();
            auto resultIsNotMoreComplex = is.second.Complexity() <= Complexity() + v.Complexity();
            is.first = is.first && resultIsNotMoreComplex;
        }

		return is;
    }

	Valuable::vars_cont_t Sum::GetVaExps() const {
        vars_cont_t vaExps;
        for (auto& m : members) {
            auto mVaExps = m.GetVaExps();
            for (auto& mve : mVaExps) {
                auto it = vaExps.find(mve.first);
                if (it == vaExps.end()) {
                    vaExps.emplace(mve);
                } else if (mve.second > it->second) {
                    it->second = mve.second;
                }
            }            
        }
        return vaExps;
	}

    Valuable Sum::Sign() const {
        auto sign = constants::zero;
        if (size()) {
            if (FindVa()) {
                return base::Sign();
            }
            OptimizeOn oo;
            std::map<Valuable, Valuable> directions;
            for (auto& m : members) {
                auto s = m.Sign();
                auto it = directions.find(s);
                if (it == directions.end()) {
                    auto ms = -s;
                    it = directions.find(ms);
                    if (it == directions.end()) {
                        directions.emplace(std::move(s), m);
                    } else {
                        it->second += m;
                        auto compensatedSign = it->second.Sign();
                        if (compensatedSign != ms) {
                            if (compensatedSign == s) {
                                directions.emplace(std::move(s),
                                    std::move(directions.extract(it).mapped()));
                            } else {
                                IMPLEMENT
                            }
                        }
                    }
                } else {
                    it->second += m;
                }
            }

            auto it = directions.begin();
            sign = it->first;
            if (directions.size() >= 1) {
                auto e = directions.end();
                while (++it != e) {
                    auto s = it->first;
                    if (-s == sign) {
                        IMPLEMENT
                    } else {
                        sign = (sign + s).Sign();
                    }
                }
            }
        }
        return sign;
    }

}
