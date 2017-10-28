//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Product.h"

#include "Exponentiation.h"
#include "Sum.h"
#include "Fraction.h"
#include <type_traits>

namespace omnn{
namespace extrapolator {
    
    // store order operator
    bool ValuableLessCompare::operator()(const Valuable& v1, const Valuable& v2)
    {
        // TODO : refactore, compare nemerical representation of order rank
        auto v1va = Variable::cast(v1);
        auto v2va = Variable::cast(v2);
        auto v1fa = v1.FindVa();
        auto v2fa = v2.FindVa();
        bool v1s = Sum::cast(v1);
        bool v2s = Sum::cast(v2);
        bool v1p = Product::cast(v1);
        bool v2p = Product::cast(v2);

        if(v1va)
        {
            if(v2va)
            {
                return v1 < v2;
            }
            else
            {
                if (v2fa)
                {
                    return true;
                }
                else
                {
                    if (v2s) {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            if(v2va)
            {
                if (v1fa)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                if(v1fa)
                {
                    return false;
                }
                else if (v2fa)
                {
                    return true;
                }
                else
                {
                    if (v1s)
                    {
                        return false;
                    }
                    else
                    {
                        if (v2s || v2p) {
                            return true;
                        }
                        else if (v1s || v1p) {
                            return false;
                        }
                        else if (Integer::cast(v1) && Exponentiation::cast(v2))
                            return true;
                        else
                            return v1 < v2;
                    }
                }
            }
        }
    }
    
    void Product::Add(const Valuable& item)
    {
        members.insert(item);
        auto v = Variable::cast(item);
        if(v)
            vars.insert(*v);
    }

	Valuable Product::operator -() const
	{
        auto it = members.begin();
        if(it == members.end())
            throw "impossible!";
        Valuable v = -*it;
        while (++it != members.end()) {
            v*=*it;
        }
        v.optimize();
        return v;
	}

    void Product::optimize()
    {
        // emerge inner products
        for (auto it = members.begin(); it != members.end();)
        {
            auto p = cast(*it);
            if (p) {
                for (auto& m : *p)
                    Add(m);
                members.erase(it++);
            }
            else  ++it;
        }

        // optimize members, if found a sum then become the sum multiplied by other members
        Sum* s = nullptr;
        for (auto it = members.begin(); it != members.end();)
        {
            s = const_cast<Sum*>(Sum::cast(*it));
            if (s)
                break;
            auto c = *it;
            c.optimize();
            if (*it != c) {
                members.erase(it++);
                members.insert(it, c);
                it = begin();
                continue;
            }
            else
                ++it;
        }
        if (s)
        {
            for (auto& it : members)
            {
                if (s == const_cast<Sum*>(Sum::cast(it)))
                    continue;
                else
                    *s *= it;
            }
            s->optimize();
            Become(std::move(*s));
            return;
        }

        // if no sum  then continue optimizing this product
        for (auto it = members.begin(); it != members.end();)
        {
            if (members.size() == 1)
            {
                Become(std::move(*const_cast<Valuable*>(&*it)));
                return;
            }

            if (*it == 1)
            {
                members.erase(it++);
                continue;
            }

            if (*it == 0)
            {
                Become(0);
                return;
            }

            bool d = false;
            auto it2 = it;
            ++it2;
            for (; it2 != members.end();)
            {
                if ((it2->OfSameType(*it) && !Variable::cast(*it))
                    || (Integer::cast(*it) && Fraction::cast(*it2))
                    || (Integer::cast(*it2) && Fraction::cast(*it))
                    )
                {
                    auto c = *it * *it2;
                    members.erase(it2++);
                    members.erase(it++);
                    d = true;
                    members.insert(it, c);
                    it = members.begin();
                    break;
                }
                else
                    ++it2;
            }

            if (d) {
                continue;
            }
            ++it;
        }
        
        // if has a fraction then do optimizations
        auto f = GetFirstOccurence<Fraction>();
        if (f != members.end()) {
            auto fo = *Fraction::cast(*f);
            // do not become a fraction for optimizations, because sum operate with products to analyse polynomial grade in FormulaOfVa
//            auto fracopy = *f;
//            *this /= *f;
//            fracopy *= *this;
//            Become(std::move(fracopy));
//            return;
            // ^ no
            
            // optimize here instead
            auto pd = Product::cast(fo.getDenominator());
            if (pd) {
                for (auto it = members.begin(); it != members.end();)
                {
                    if (pd->Has(*it)) {
                        auto el = std::move(*it);
                        *const_cast<Product*>(pd) /= el;
                        members.erase(it++);
                    }
                    else  ++it;
                }
            }
            
            fo.optimize();
            
            // check if it still fraction
            auto ff = Fraction::cast(fo);
            if (ff) {
                auto dn = ff->getDenominator();
                pd = Product::cast(dn);
                if (!pd) {
                    for (auto it = members.begin(); it != members.end();)
                    {
                        if (dn == *it) {
                            members.erase(it++);
                            fo *= dn;
                            fo.optimize();
                            // not a fraction any more
                            break;
                        }
                        else  ++it;
                    }
                }
            }
            
            if(*f != fo)
            {
                members.erase(f++);
                members.insert(f,fo);
            }
        }
    }

    const std::multiset<Variable>& Product::getCommonVars() const
    {
        return vars;
    }
    
    Valuable Product::varless() const
    {
        Valuable p(1);
        for(auto& m : members)
        {
            auto v = Variable::cast(m);
            if(!v)
                p*=m;
        }
        return p;
    }
    
    Valuable& Product::operator +=(const Valuable& v)
    {
        auto p = cast(v);
        if (p)
        {
            if(*this == -v)
                return Become(0_v);

            auto cv = getCommonVars();
            if (!cv.empty() && cv == p->getCommonVars())
            {
                auto valuable = varless() + p->varless();
                for (auto& va : vars)
                    valuable *= va;
                return Become(std::move(valuable));
            }
        }
        return Become(Sum(*this, v));
    }

	Valuable& Product::operator +=(int v)
	{
		return *this += Valuable(v);
	}

    Valuable& Product::operator *=(const Valuable& v)
    {
        auto s = Sum::cast(v);
        if (s)
            return Become(v**this);

        auto va = Variable::cast(v);
        if (!va)
        {
            for (auto it = members.begin(); it != members.end();)
            {
                if (it->OfSameType(v)) {
                    auto m = *it*v;
                    members.erase(it++);
                    members.insert(it,m);
                    goto yes;
                }
                else  ++it;
            }
        }

        // add new member
        Add(v);

    yes:
        optimize();
        return *this;
    }

	Valuable& Product::operator /=(const Valuable& v)
	{
        auto e = Exponentiation::cast(v);
        for (auto it = members.begin(); it != members.end(); ++it)
        {
            if (*it == v)
            {
                members.erase(it);
                auto va = Variable::cast(v);
                if (va) {
                    vars.erase(*va);
                }
                return *this;
            }
            else if (e && *it == e->getBase())
            {
                *this /= e->getBase();
                *this /= *e / e->getBase();
                return *this;
            }
        }
        
        return *this *= Fraction(1, v);
	}

	Valuable& Product::operator %=(const Valuable& v)
	{
		return base::operator %=(v);
	}

	Valuable& Product::operator --()
	{
		return Become(Sum(this, -1));
	}

	Valuable& Product::operator ++()
	{
		return Become(Sum(this, 1));
	}

	bool Product::operator <(const Valuable& v) const
	{
		// not implemented comparison to this Valuable descent
		return base::operator <(v);
	}

	bool Product::operator ==(const Valuable& v) const
	{
        auto p = cast(v);
        bool eq = p && Hash() == p->Hash() && members==p->members;
        return eq;
	}

	std::ostream& Product::print(std::ostream& out) const
	{
        std::stringstream s;
        constexpr char sep[] = "*";
        for (auto& b : members)
            s << b << sep;
        auto str = s.str();
        auto cstr = const_cast<char*>(str.c_str());
        cstr[str.size() - sizeof(sep) + 1] = 0;
        out << cstr;
        return out;
	}

    const Variable* Product::FindVa() const
    {
        return vars.size() ? &*vars.begin() : nullptr;
    }

    void Product::Eval(const Variable& va, const Valuable& v)
    {
        // eval members
        base::Eval(va, v);

        if (cast(*this) // still Product
            && vars.size())
        {
            bool found;
            do
            {
                auto it = vars.find(va);
                found = it != vars.end();
                if (found)
                    vars.erase(it);
            } while (found);
        }
    }
}}
