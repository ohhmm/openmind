//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Sum.h"
#include "e.h"
#include "Formula.h"
#include "Fraction.h"
#include "i.h"
#include "Infinity.h"
#include "Integer.h"
#include "Modulo.h"
#include "Product.h"
#include "pi.h"
#include "System.h"
#include "VarHost.h"
#include "Cache.h"

#include <algorithm>
#include <cmath>
#ifdef _WIN32
#include <execution>
#endif
#include <future>
#include <map>
#include <stack>
#include <thread>
#include <type_traits>

#ifdef OPENMIND_USE_OPENCL
#include <boost/compute.hpp>
#endif
//TODO:
//import std;

namespace omnn{
namespace math {

    CACHE(DbSumOptimizationCache);
    CACHE(DbSumSolutionsOptimizedCache);
    CACHE(DbSumSolutionsAllRootsCache);
    CACHE(DbSumSolutionsARootCache);

    namespace
    {
        using namespace std;
        
        type_index order[] = {
            typeid(MInfinity),
            typeid(Sum),
            typeid(Product),
            typeid(Exponentiation),
            typeid(Variable),
            typeid(Euler),
            typeid(Pi),
            typeid(MinusOneSq),
            typeid(Integer),
            typeid(Fraction),
            typeid(Modulo),
            typeid(Infinity),
        };
        
        auto ob = std::begin(order);
        auto oe = std::end(order);
        
        // inequality should cover all cases
        auto toc = [](const Valuable& x, const Valuable& y) // type order comparator
        {
            auto it1 = std::find(ob, oe, static_cast<type_index>(x));
            assert(it1!=oe); // IMPLEMENT
            auto it2 = std::find(ob, oe, static_cast<type_index>(y));
            assert(it2!=oe); // IMPLEMENT
            return it1 == it2 ? *it1 > *it2 : it1 < it2;
        };
        
        SumOrderComparator soc;
    }
    
    // store order operator
    bool SumOrderComparator::operator()(const Valuable& v1, const Valuable& v2) const
    {
        return v1.OfSameType(v2)
            || (v1.IsProduct() && v2.IsExponentiation())
            || (v2.IsProduct() && v1.IsExponentiation())
            ? v1.IsComesBefore(v2) : toc(v1,v2);
    }

    Sum::iterator Sum::Had(iterator it)
    {
        //return it;
        auto item = *it;
        std::cout << item.str();
        it = std::find(members.begin(), members.end(), item);
        throw "Impossible! Check order comparator error.";
        return it;
    }

    auto Thr = std::thread::hardware_concurrency() << 3;
    
    const Sum::iterator Sum::Add(const Valuable& item, const iterator hint)
    {
        Sum::iterator it = end();
        if(item.IsSum()) {
            auto s = cast(item);
            for(auto& i : *s) {
                auto a = Add(i);
                if (it == end() || soc(*a, *it)) {
                    it = a;
                }
            }
            it = members.begin();
        }
        else
        {
#ifdef _WIN32
            if (members.size() > Thr)
                it = std::find(std::execution::par, members.begin(), members.end(), item);
            else
#endif
                it = members.find(item);

            if(it==end())
                it = base::Add(item, hint);
            else
                Update(it, item*2);

            auto itemMaxVaExp = item.getMaxVaExp();
            if(itemMaxVaExp > maxVaExp)
                maxVaExp = itemMaxVaExp;
        }
        return it;
    }
    
	Valuable Sum::operator -() const
	{
		Sum s;
		for (auto& a : members) 
			s.Add(-a);
		return s;
	}

    Valuable Sum::GCD() const {
        auto it = members.begin();
        auto gcd = it->varless();
        for (; it != members.end(); ++it) {
            gcd = boost::gcd(gcd, it->varless());
        }
        return gcd;
    }
    
    void Sum::optimize()
    {
        if (optimized || !optimizations) return;

        if (isOptimizing)
            return;
        isOptimizing = true;

        auto s = str();
        auto doCheck = s.length() > 10;
        auto checkCache = doCheck ? DbSumOptimizationCache.AsyncFetch(*this, true) : Cache::Cached();

        Valuable w;
        do
        {
            if (checkCache) {
                Become(checkCache);
                return;
            }
            w = *this;
            if (checkCache) {
                Become(checkCache);
                return;
            }
            
            if (members.size() == 1) {
                Valuable m;
                {m = *members.begin();}
                Become(std::move(m));
                return;
            }
            if (checkCache) {
                Become(checkCache);
                return;
            }
            
            for (auto it = members.begin(); it != members.end();)
            {
                // optimize member
                auto copy = *it;
                copy.optimize();
                if (copy == 0)
                    Delete(it);
                else if (!it->Same(copy))
                    Update(it, copy);
                else
                    ++it;
                
                if (checkCache) {
                    Become(checkCache);
                    return;
                }
            }
            
            if (view == Equation) {
                auto& coVa = getCommonVars();
                if (coVa.size()) {
                    *this /= VaVal(coVa);
                    if (!IsSum()) {
                        isOptimizing = {};
                        return;
                    }
                }
            }
            
            if (checkCache) {
                Become(checkCache);
                return;
            }
            
            for (auto it = members.begin(); it != members.end();)
            {
                if (it->IsSum()) {
                    for (auto& m : cast(*it)->members)
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
                    if (checkCache) {
                        Become(checkCache);
                        return;
                    }
                    
                    if(c.IsSum()){
                        IMPLEMENT
                    }
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
                        c = 0_v;
                        Delete(it2);
                        up();
                    }
                    else if (comVaEq())
                    {
                        auto sum = c.IsProduct() ? c : Product{c};
                        sum += *it2;
                        if (!sum.IsSum()) {
                            c = sum;
                            Delete(it2);
                            up();
                        } else
                            ++it2;
                    }
                    else if ((inc = it2->InCommonWith(c)) != 1_v
                             && inc.IsMultival() != YesNoMaybe::Yes) {
                        thread_local bool antiloop = false;
                        if (!antiloop) {
                            antiloop = true;
                            auto sum = c / inc + *it2 / inc;
                            if(!sum.IsSum())
                            {
                                sum *= inc;
                                c = sum;
                                Delete(it2);
                                up();
                            } else
                                ++it2;
                            antiloop = false;
                        } else
                            ++it2;
                    }
                    else if (c.IsFraction() && it2->IsFraction()
                             && Fraction::cast(c)->getDenominator() == Fraction::cast(*it2)->getDenominator())
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
                if (checkCache) {
                    Become(checkCache);
                    return;
                }
                auto copy = *it;
                
                if (checkCache) {
                    Become(checkCache);
                    return;
                }
                copy.optimize();
                
                if (checkCache) {
                    Become(checkCache);
                    return;
                }
                
                if (!it->Same(copy)) {
                    Update(it, copy);
                }
                else
                    ++it;
            }
            
#ifndef NDEBUG
            if (w!=*this) {
//                std::cout << "Sum optimized from \n\t" << w << "\n \t to " << *this << std::endl;
            }
#endif
        } while (w != *this);

        if(IsSum())
        {
            if (members.size() == 0) {
                Become(0_v);
            }
            else if (members.size() == 1) {
                cont::iterator b = members.begin();
                Become(std::move(const_cast<Valuable&>(*b)));
            }
            else if (view==Solving || view == Equation){
                // make coefficients int to use https://simple.wikipedia.org/wiki/Rational_root_theorem
                bool scan;
                do {
                    scan = {};
                    if (IsSum())
                    {
                        for (auto& m : members)
                        {
                            if (m.IsProduct()) {
                                auto p = Product::cast(m);
                                for (auto& m : *p) {
                                    if (m.IsFraction()) {
                                        operator*=(Fraction::cast(m)->getDenominator());
                                        scan = true;
                                        break;
                                    }
                                    else if (m.IsExponentiation()) {
                                        auto e = Exponentiation::cast(m);
                                        auto& ee = e->getExponentiation();
                                        if (ee.IsInt() && ee < 0) {
                                            operator*=(e->getBase() ^ (-ee));
                                            scan = true;
                                            break;
                                        }
                                        else if (ee.IsFraction()) {
                                            auto f = Fraction::cast(ee);
                                            auto& d = f->getDenominator();
                                            auto wo = *p / *e;
                                            Become(((e->getBase() ^ f->getNumerator()) * (wo^d)) - ((-(*this - *p)) ^ d));
                                            scan = true;
                                            break;
                                        }
                                    }
                                }
                                if (scan)
                                    break;
                            }
                            else if (m.IsFraction()) {
                                operator*=(Fraction::cast(m)->getDenominator());
                                scan = true;
                                break;
                            }
                            else if (m.IsExponentiation()) {
                                auto e = Exponentiation::cast(m);
                                auto& ee = e->getExponentiation();
                                if (ee.IsInt() && ee < 0) {
                                    operator*=(e->getBase() ^ (-ee));
                                    scan = true;
                                    break;
                                }
                                else if (ee.IsFraction()) {
                                    auto f = Fraction::cast(ee);
                                    Become((e->getBase() ^ f->getNumerator()) - ((-(*this - m)) ^ f->getDenominator()));
                                    scan = true;
                                    break;
                                }
                            }
                        }
                    }
                } while (scan);
                
                if(IsSum())
                {
                    auto gcd = GCD();
                    if(gcd!=1_v){
                        operator/=(gcd);
                    }
                    // todo : make all va exponentiations > 0
                } else
                    return;
            }
            
            if (view == Equation) {
                auto& coVa = getCommonVars();
                if (coVa.size()) {
                    *this /= VaVal(coVa);
                    if (!IsSum()) {
                        return;
                    }
                }
            }
        }

        if (doCheck && checkCache.NotInCache()) {
            DbSumOptimizationCache.AsyncSet(std::move(s), str());
        }
        
        if (IsSum()) {
            isOptimizing = false;
            optimized = true;
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
                                              [](auto& _1, auto& _2){return _1.abs() < _2.abs();});
                        if ((i->second < 0) == (found->second < 0)) {
                            i->second = mm.first;
                        }
                        else
                        {
                            auto sign = mm.second / mm.second.abs();
                            i->second = sign * (mm.second.abs() - mm.first.abs());
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
        if(v.IsSum())
            IMPLEMENT;
        auto c = v;
        auto b = GetConstCont().begin();
        auto e = GetConstCont().end();
        if(b != e)
        {
            auto it = b;
            while(c != 1_v && ++it != e)
                c = it->InCommonWith(c);
        }
        return c;
    }
    
    Valuable& Sum::operator +=(const Valuable& v)
    {
        if (v.IsInt() && v == 0) {
            return *this;
        }
        if (v.IsSum()) {
            auto s = cast(v);
            for (auto& i : *s) {
                operator+=(i);
            }
        }
        else
        {
            for (auto it = members.begin(); it != members.end(); ++it)
            {
                if (it->OfSameType(v) && it->getCommonVars() == v.getCommonVars())
                {
                    auto s = *it + v;
                    if (!s.IsSum()) {
                        Update(it, s);
                        optimize();
                        return *this;
                    }
                }
            }

            // add new member
            Add(v);
        }

        optimize();
        return *this;
    }

	Valuable& Sum::operator *=(const Valuable& v)
	{
        Sum sum;
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
        else if (v.IsSum())
            for (auto& _1 : *cast(v))
                for (auto& _2 : members)
                    sum.Add(_1 * _2);
        else
        {
            for (auto& a : members)
            {
                sum.Add(a * v);
            }
            if (vIsInt)
                sum.optimized = optimized;
        }
        Become(std::move(sum));
        return *this;
	}

	Valuable& Sum::operator /=(const Valuable& v)
	{
        Valuable s = 0_v;
        auto view = GetView();
        SetView(None);
		if (v.IsSum())
		{
            auto i = cast(v);
            if(!v.FindVa())
            {
                for (auto& a : members) {
                    for (auto& b : i->members) {
                        s += a / b;
                    }
                }
            }
            else
            {
                if (size() < i->size())
                {
                    s = Fraction(*this, v);
                }
                else if (HasSameVars(v))
                {
                    auto b=i->begin();
                    auto e = i->end();
                    size_t offs = 0;
                    std::deque<Valuable> hist {*this};
                    auto icnt = size() * 2;
                    while(*this != 0_v && icnt--)
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
                            auto coVa = it->getCommonVars();
                            auto maxVa = std::max_element(coVa.begin(), coVa.end(),
                                                          [](auto&_1,auto&_2){return _1.second < _2.second;});
                            auto it2 = b;
                            while(it2 != e && it2->Vars() != vars)
                                ++it2;
                            if (it2 == e) {
                                
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
                    if (*this != 0_v) {
                        s = Fraction(*this, v);
                    }
                }
                else
                {
                    s = Fraction(*this, v);
                }
            }
		}
		else
		{
            for (auto& a : members) {
                s += a / v;
            }
		}
        s.SetView(view);
        SetView(view);
        return Become(std::move(s));
	}

    Valuable& Sum::operator %=(const Valuable& v)
    {
        Valuable s = 0_v;
        if (v.IsSum())
        {
            auto i = cast(v);
            if(!v.FindVa())
            {
                for (auto& a : members) {
                    for (auto& b : i->members) {
                        s += a / b;
                    }
                }
            }
            else
            {
                if (size() < i->size())
                {
                    s = Fraction(*this, v);
                }
                else if (HasSameVars(v))
                {
                    auto b=i->begin();
                    auto e = i->end();
                    size_t offs = 0;
                    std::deque<Valuable> hist {*this};
                    
                    auto icnt = size() * 2;
                    while(*this != 0_v && icnt--)
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
                            auto coVa = it->getCommonVars();
                            auto maxVa = std::max_element(coVa.begin(), coVa.end(),
                                                          [](auto&_1,auto&_2){return _1.second < _2.second;});
                            auto it2 = b;
                            while(it2 != e && it2->Vars() != vars)
                                ++it2;
                            if (it2 == e) {
                                
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
                            return Become(Modulo(*this, v));
                        }
                    }
                    if (*this != 0_v) {
                        s = Modulo(*this, v);
                    }
                }
                else
                {
                    s = Modulo(*this, v);
                }
            }
        }
        else
        {
            for (auto& a : members) {
                s += a % v;
            }
            s = Modulo(s,v);
        }
        return Become(std::move(s));
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
        Valuable sum = 0_v;
        auto add = [&](const Valuable& m)
        {
            if (sum.IsSum()) {
                Sum::cast(sum)->Add(m);
            }
            else
                sum += m;
        };
        for(auto m : *this)
            add(m.d(x));
        
        return Become(std::move(sum));
    }
    
    Sum::Sum(const std::initializer_list<Valuable>& l)
    {
        for (const auto& arg : l)
        {
            auto a = cast(arg);
            if(a)
                for(auto& m: *a)
                    this->Add(m, end());
            else
                this->Add(arg, end());
        }
    }
    
    bool Sum::IsComesBefore(const Valuable& v) const
    {
        if (v.IsSum()) {
            auto sz1 = size();
            auto s = Sum::cast(v);
            auto sz2 = s->size();
            if (sz1 != sz2) {
                return sz1 > sz2;
            }
            
            for (auto i1=begin(), i2=s->begin(); i1!=end(); ++i1, ++i2) {
                if (*i1 != *i2) {
                    return i1->IsComesBefore(*i2);
                }
            }
        }
        
        return {};
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
    
    Valuable Sum::Sqrt() const
    {
        IMPLEMENT // TODO : try to get rid of this call instead by substituting to ^(1_v/2) which is not equivalent to sqrt by the way https://math.stackexchange.com/questions/41784/convert-any-number-to-positive-how/41787#comment5776496_41787
        return Exponentiation(*this, 1_v/2); // TODO  :  this is wrong  because of https://math.stackexchange.com/questions/41784/convert-any-number-to-positive-how/41787#comment5776496_41787
    }

    Valuable& Sum::sq()
    {
        Sum s;
        auto e = end();
        for (auto i = begin(); i != e; ++i)
        {
            s.Add(i->Sq());
            for (auto j = i; ++j != e;)
            {
                s.Add(*i * *j * 2);
            }
        }
        return Become(std::move(s));
    }

    Valuable Sum::calcFreeMember() const
    {
        Valuable _ = 0_v;
        for(auto& m : *this) {
            _ += m.calcFreeMember();
        }
        return _;
    }

    size_t Sum::FillPolyCoeff(std::vector<Valuable>& coefficients, const Variable& v) const
    {
        auto wasopt=optimizations;
        optimizations=true;
        size_t grade = 0;
        auto sz = static_cast<size_t>((Valuable(getMaxVaExp())+1).ca());
        coefficients.resize(sz);
        Sum c0;
        auto add = [&](auto i, Valuable&& a) {
            if (i) {
                coefficients[i] += a;
            } else
                c0.Add(a);
        };
	    // TODO : openmp
	    //#pragma omp parallel default(none) shared(grade,coefficients)
	    {
        //#pragma omp for 
        for (auto& m : members)
        {
            if (m.IsProduct())
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
                        auto s = *this;
                        s.SetView(View::Solving);
                        s.optimize();
                        coefficients.clear();
                        return s.FillPolyCoeff(coefficients, v);
                    }
                }

                auto vcnt = noVa ? 0 : it->second; // exponentation of va
                if (!vcnt.IsInt()) {
                    IMPLEMENT
                }
                int ie = static_cast<int>(vcnt);
                if (ie < 0)
                {
                    coefficients.clear();
                    return cast(*this * (v ^ (-ie)))->FillPolyCoeff(coefficients, v);
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
                ++coefficients[1];
                if(grade < 1)
                    grade = 1;
            }
            else if(m.IsExponentiation())
            {
                auto e = Exponentiation::cast(m);
                if (e && e->getBase()==v)
                {
                    auto& ee = e->getExponentiation();
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
            else if(!m.HasVa(v))
            {
                c0.Add(m);
            }
            else {
                std::cout << m << std::endl;
                IMPLEMENT
            }
        }
        }
        if(c0.size()){
//            c0.optimized = optimized;
            coefficients[0] = std::move(c0);
        }
        optimizations=wasopt;
        return grade;
    }
    
    Valuable Sum::operator()(const Variable& va, const Valuable& augmentation) const
    {
        solutions_t solutions;
        {
        auto e = *this + augmentation;
        if (!e.IsSum()) {
            IMPLEMENT;
        }
        auto es = cast(e);
        std::vector<Valuable> coefs;
        auto grade = es->FillPolyCoeff(coefs, va);
        if (coefs.size() && grade && grade <= 2)
        {
            es->solve(va, solutions, coefs, grade);
            for (auto i=solutions.begin(); i != solutions.end();) {
                if (i->HasVa(va)) {
                    IMPLEMENT
                    solutions.erase(i++);
                }
                else
                    ++i;
            }
            
            if (solutions.size()) {
                return Valuable(solutions);
            }
        }
        }
        Valuable _ = augmentation;
        Valuable todo = 0_v;
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
                    if(augmentation == 0)
                        solutions.insert(0);
                    else
                        IMPLEMENT
                }
                if (it->second < 0) {
                    _ *= it->first ^ -it->second;
                    return _(va, todo);
                }
                else if (todo != 0_v)
                {
                    Valuable solution = Fraction(_, todo);
                    if (it->second != 1_v) {
                        solution ^= 1_v/it->second;
                    }
                    else
                        solution.optimize();
                    solutions.insert(solution);
                }
            }
            else
            {
                auto stodo = cast(todo);
                if (stodo->size() == 2)
                {
                    if(stodo->begin()->IsFraction())
                    {
                        return (*stodo->begin())(va, _ - *stodo->rbegin());
                    }
                    else if(stodo->rbegin()->IsFraction())
                    {
                        return (*stodo->rbegin())(va, _ - *stodo->begin());
                    }
                    else
                    {
                        IMPLEMENT
                    }
                }
                else
                {
                    solutions.insert(0_v);
                    for(auto& m : *stodo)
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

        return Valuable(solutions);
    }
    
    Valuable Sum::operator()(const Variable& va) const
    {
        Valuable::solutions_t s;
        
        std::vector<Valuable> coefs;
        auto grade = FillPolyCoeff(coefs, va);
        if(!grade){
#ifndef NDEBUG
            grade = FillPolyCoeff(coefs, va);
#endif
            return Valuable(s);
        }
        
        auto doCheck = grade > 2;
        auto checkCached = doCheck
                            ? DbSumSolutionsOptimizedCache.AsyncFetch(*this, true)
                            : Cache::Cached();
        if (grade == 2) {
            auto& a = coefs[2];
            auto& b = coefs[1];
            auto& c = coefs[0];
            auto d = (b ^ 2) - 4_v * a * c;
            return ((d^(1_v/2))-b)/(a*2);
        } else if (grade == 3) {
            //            auto de = *this;  de.d(va);
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
            return Valuable(Solutions(va));
        }
        else if (coefs.size())// && grade && grade < 3)
        {
            if(checkCached)
                return checkCached;
            solve(va, s, coefs, grade);
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

            if(!coefs[0]) // no free coef -> zero root
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
                    return Valuable(s);
                }
                else
                {
                    // TODO : IMPLEMENT
                }
            }
            
            // TODO : IMPLEMENT, the next here needs debugging
            Valuable augmentation = 0_v;
            Valuable _ = 0_v;
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
        
        Valuable pluralSolutionsExpression(s);
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
        auto sum = cast(_);
        if (!sum) {
            IMPLEMENT
        }
        
        std::vector<Valuable> coefficients;
        auto g = sum->FillPolyCoeff(coefficients,va);
        if (g<3)
        {
            sum->solve(va, solutions, coefficients, g);
            
            if(solutions.size())
            {
                return solutions;
            }
        }

        solutions_t zs;
        auto zz = sum->get_zeros_zones(va, zs);
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
                                        
                                        auto found = _ == 0_v;
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
                                            if(!haveMin || _.abs() < min.abs()) {
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
        auto i = Integer::cast(freeMember);
        if(!i) {
            IMPLEMENT
        }
        
        if (finder(i)) {
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
//            auto t = *this;
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
                    if(d>0){
                        auto dsq = d.Sqrt();
                        solutions.insert((-dsq-b)/a2);
                        solutions.insert((dsq-b)/a2);
                    } else if (d==0) {
                        solutions.insert(-b/a2);
                    } else {
                        solutions.insert(((d^(1_v/2))-b)/a2);
                    }
                    break;
                }
            }
            default: {
                // RATIONAL ROOT TEST
                auto checkCache = DbSumSolutionsAllRootsCache.AsyncFetchSet(*this);
                if(checkCache){
                    solutions = checkCache;
                    return;
                }
                if(GetView() != View::Solving && GetView() != View::Equation) {
//                    auto 
                }
                auto a = coefficients[grade];
                auto k = coefficients[0];
                if(!k.IsInt()) {
#ifndef NDEBUG
                    std::cout << "free member needed optimization: " << k << std::endl;
#endif
                    k.optimize();
                }
                if(!(a.IsInt() && k.IsInt())) {
                    IMPLEMENT
                } else {
                    Valuable test;
                    auto ai = Integer::cast(a);
                    auto ki = Integer::cast(k);
                    auto found = ai->Factorization([&](const auto& i){
                        return i!=0
                            && ki->Factorization([&](const auto& ik)->bool{
                                test = ik / i;
                                return Test(va, test) || Test(va, test=-test);
                                }, Infinity());
                        }, Infinity());
                    if(checkCache){
                        solutions = checkCache;
                        return;
                    }
                    if(found) {
                        auto restToSolve = *this / va.Equals(test);
                        if(checkCache){
                            solutions = checkCache;
                            return;
                        }
                        restToSolve.solve(va, solutions);
                        solutions.insert(std::move(test));
                        if(checkCache.NotInCache())
                            DbSumSolutionsAllRootsCache.AsyncSetSet(*this, solutions);
                        return;
                    }
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
//                auto co = *this;
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
                if (di == 0)
                {
                    if (d0 == 0)
                    {
                        solutions.insert(b / (a*-3));
                    }
                    else
                    {
                        solutions.insert((a*d * 9 - b * c) / (d0 * 2));
                        solutions.insert((a*b*c * 4 + asq * d*-9 - bsq * b) / (d0 * a));
                    }
                }
                else
                {
                    auto d1 = (bsq * 2 - ac3 * 3)*b + asq * d * 27; // +
                    auto subC = (asq*di*-27).Sqrt();
                    auto C = ((d1+((d1.Sq()-d0.Sq()*d0*4)^(1_v/2)))/2)^(1_v/3); // +
                    auto _1 = -1_v/(a*3); // +
                    auto _2 = b+C+(d0/C); // +
                    auto x = _1*_2; // complex with real part that is root
                    if(Test(va,x)) // << error in Eval
                    {
                        (*this / va.Equals(x)).solve(va, solutions);
                        solutions.insert(std::move(x));
                        return;
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
                auto qp2 = (p2/2)^(1_v/3);
                p1.optimize();
                p2.optimize();
                qp2.optimize();
                auto p3 = (c*c-3*b*d+12*a*e)/(3*a*qp2)+qp2/(3*a);
                auto p4 = (sb/(4*sa)-(2*c)/(3*a)+p3).Sqrt();
                auto p5 = sb/(2*sa)-(4*c)/(4*a)-p3;
                auto p6 = (-sb*b/(sa*a)+4*b*c/sa-8*d/a)/(4*p4);
                auto xp1 = b/(-4*a);
                auto xp2 = p4/2;
                auto xp3_1 = (p5-p6).Sqrt()/2;
                auto xp3_2 = (p5+p6).Sqrt()/2;
                solutions.insert(xp1-xp2-xp3_1);
                solutions.insert(xp1-xp2+xp3_1);
                solutions.insert(xp1+xp2-xp3_2);
                solutions.insert(xp1+xp2+xp3_2);
                break;
            }
            default: {
                // build OpenCL kernel
#ifdef OPENMIND_USE_OPENCL
				using namespace boost::compute;
                auto copy = *this;
                copy.optimize();
                std::stringstream source;
                source << "__kernel void f(__global long *c) {"
                        << "    const uint i = get_global_id(0);"
                        << "    long " << va << "=i;"
                        << "    c[i] = "; copy.code(source);
                source << ";}";
                
                device cuwinner = system::default_device();
                for(auto& p: system::platforms())
                    for(auto& d: p.devices())
                        if (d.compute_units() > cuwinner.compute_units())
                            cuwinner = d;
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
                
                auto simple = *this;
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
                        if (copy == 0_v) {
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
                auto teqs = Sum::cast(teq);
                if (teqs) {
                    if (teqs->FillPolyCoeff(teq_coefficients, va) != grade) {
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
                auto qp2 = (p2/2)^(1_v/3);
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
        auto is = IsSumationSimplifiable(v);
        if(is.first)
            Become(std::move(is.second));
        return is.first;
    }

    std::pair<bool,Valuable> Sum::IsSumationSimplifiable(const Valuable& v) const{
        // TODO : optimize
        auto m = *this + v;
        return { m.Complexity() < Complexity() + v.Complexity(), m };
    }
}}
