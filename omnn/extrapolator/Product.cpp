//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Product.h"

#include "Fraction.h"
#include "Integer.h"
#include "Sum.h"

#include <type_traits>

namespace omnn{
namespace extrapolator {
    
    void Product::AddToVarsIfVaOrVaExp(const Valuable &item)
    {
        auto v = Variable::cast(item);
        if(v)
        {
            vars[*v] += 1;
            if (vars[*v] == 0) {
                vars.erase(*v);
            }
        }
        else
        {
            auto e = Exponentiation::cast(item);
            if (e)
            {
                v = Variable::cast(e->getBase());
                if (v) {
                    vars[*v] += e->getExponentiation();
                    if (vars[*v] == 0) {
                        vars.erase(*v);
                    }
                }
            }
        }
    }
    
    void Product::Add(const Valuable& item)
    {
        base::Add(item);
        AddToVarsIfVaOrVaExp(item);
    }
    
    void Product::Update(typename cont::iterator& it, const Valuable& v)
    {
        base::Update(it,v);
        AddToVarsIfVaOrVaExp(v);
    }

    void Product::Delete(typename cont::iterator& it)
    {
        auto v = Variable::cast(*it);
        if(v)
        {
            vars[*v] -= 1;
            if (vars[*v] == 0) {
                vars.erase(*v);
            }
        }
        else
        {
            auto e = Exponentiation::cast(*it);
            if (e)
            {
                v = Variable::cast(e->getBase());
                if (v) {
                    vars[*v] -= e->getExponentiation();
                    if (vars[*v] == 0) {
                        vars.erase(*v);
                    }
                }
            }
        }
        base::Delete(it);
    }
    
	Valuable Product::operator -() const
	{
        Product p = *this;
        auto b = p.begin();
        p.Update(b, -*b);
        return p;
	}

    void Product::optimize()
    {
        //if (!optimizations) return;
        
        // optimize members, if found a sum then become the sum multiplied by other members
        Sum* s = nullptr;
        for (auto it = members.begin(); it != members.end();)
        {
            s = const_cast<Sum*>(Sum::cast(*it));
            if (s)
                break;
            auto c = *it;
            c.optimize();
            if (!it->Same(c)) {
                Update(it, c);
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

        
        // if no sum then continue optimizing this product
        
        // emerge inner products
        for (auto it = members.begin(); it != members.end();)
        {
            auto p = cast(*it);
            if (p) {
                for (auto& m : *p)
                    Add(m);
                Delete(it);
            }
            else
                ++it;
        }
        
        for (auto it = members.begin(); it != members.end();)
        {
            if (members.size() == 1)
            {
                Become(std::move(*const_cast<Valuable*>(&*it)));
                return;
            }

            if (*it == 1)
            {
                Delete(it);
                continue;
            }

            if (*it == 0)
            {
                Become(0);
                return;
            }

            auto c = *it;
            auto e = Exponentiation::cast(c);
            auto f = Fraction::cast(c);
            auto i = Integer::cast(c);
            auto v = Variable::cast(c);
            auto it2 = it;
            ++it2;
            for (; it2 != members.end();)
            {
                const Exponentiation* e2 = Exponentiation::cast(*it2);
                if ((it->OfSameType(*it2) && (i || f))
                    || (v && v->Same(*it2))
                    || (i && Fraction::cast(*it2))
                    || (Integer::cast(*it2) && f)
                    || (v && e2 && e2->getBase() == c)
                    || (e && e->getBase() == *it2 && Variable::cast(*it2))
                    || (e && e2 && e->getBase() == e2->getBase())
                    )
                {
                    c *= *it2;
                    Delete(it2);
                    e = Exponentiation::cast(c);
                    f = Fraction::cast(c);
                    i = Integer::cast(c);
                    v = Variable::cast(c);
                    break;
                }
                else if (it->Same(*it2) && v)
                {
                    c ^= 2;
                    Delete(it2);
                    e = Exponentiation::cast(c);
                    f = Fraction::cast(c);
                    i = Integer::cast(c);
                    v = Variable::cast(c);
                    break;
                }
                else
                    ++it2;
            }

            if (!it->Same(c)) {
                Update(it, c);
            }
            else
                ++it;
        }
        
        // fraction optimizations
        auto f = GetFirstOccurence<Fraction>();
        if (f != members.end()) {
            Valuable fo = *Fraction::cast(*f);
            // do not become a fraction for optimizations, because sum operate with products to analyse polynomial grade in FormulaOfVa
            // optimize here instead
            auto dn = Fraction::cast(fo)->getDenominator();
            auto pd = Product::cast(dn);
            if (pd) {
                for (auto it = members.begin(); it != members.end();)
                {
                    if (pd->Has(*it)) {
                        dn /= *it;
                        Delete(it);
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
                            Delete(it);
                            fo *= dn;
                            fo.optimize();
                            // not a fraction any more
                            break;
                        }
                        else  ++it;
                    }
                }
            }
            
            if(!f->Same(fo))
            {
                Update(f,fo);
            }
        }
        
        if(members.size()==0)
            Become(1_v);
    }

    const Product::vars_cont_t& Product::getCommonVars() const
    {
        return vars;
    }
    
    Product::vars_cont_t Product::getCommonVars(const vars_cont_t& with) const
    {
        vars_cont_t common;
        for(auto& kv : vars)
        {
            auto it = with.find(kv.first);
            if (it != with.end()) {
                auto i1 = Integer::cast(kv.second);
                auto i2 = Integer::cast(it->second);
                if (i1 && i2 && *i1 >= 0_v && *i2 >= 0_v) {
                    common[kv.first] = std::min(*i1, *i2);
                }
                else
                {
                    throw "Implement!";
                }
            }
        }
        return common;
    }
    
    Valuable Product::varless() const
    {
        return *this / getVaVal();
    }
    
    Valuable Product::VaVal(const vars_cont_t& v)
    {
        Valuable p(1);
        for(auto& kv : v)
        {
            p *= kv.first ^ kv.second;
        }
        p.optimize();
        return p;
    }
    
    Valuable Product::getVaVal() const
    {
        return VaVal(vars);
    }
    
    Valuable Product::getCommVal(const Product& with) const
    {
        return VaVal(getCommonVars(with.getCommonVars()));
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
                valuable *= getVaVal();
                return Become(std::move(valuable));
            }
        }
        return Become(Sum(*this, v));
    }

    Valuable& Product::operator *=(const Valuable& v)
    {
        auto s = Sum::cast(v);
        if (s)
            return Become(v**this);

        auto va = Variable::cast(v);
        if (va)
        {
            for (auto it = members.begin(); it != members.end();)
            {
                if (it->Same(v)) {
                    Update(it, Exponentiation(*va, 2));
                    goto yes;
                }
                else
                {
                    auto e = Exponentiation::cast(*it);
                    if (e && e->getBase() == *va) {
                        Update(it, Exponentiation(*va, e->getExponentiation()+1));
                        goto yes;
                    }
                    else
                        ++it;
                }
            }
        }
        else
        {
            auto e = Exponentiation::cast(v);
            if(e)
            {
                for (auto it = members.begin(); it != members.end();)
                {
                    auto ite = Exponentiation::cast(*it);
                    if (ite && ite->getBase() == e->getBase())
                    {
                        Update(it, *it*v);
                        goto yes;
                    }
                    else
                    {
                        if (e->getBase() == *it) {
                            Update(it, Exponentiation(e->getBase(), e->getExponentiation()+1));
                            goto yes;
                        }
                        else
                            ++it;
                    }
                }
            }
            else
            {
                for (auto it = members.begin(); it != members.end();)
                {
                    if (it->OfSameType(v)) {
                        Update(it, *it*v);
                        goto yes;
                    }
                    else
                        ++it;
                }
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
        if (v == 1_v) {
            return *this;
        }
        auto p = Product::cast(v);
        if (p) {
            for(auto& i : *p)
            {
                *this /= i;
            }
            optimize();
            return *this;
        }
        else
        {
            auto e = Exponentiation::cast(v);
            for (auto it = members.begin(); it != members.end(); ++it)
            {
                if (*it == v)
                {
                    Delete(it);
                    optimize();
                    return *this;
                }
                else if (e && *it == e->getBase())
                {
                    *this /= e->getBase();
                    *this /= *e / e->getBase();
                    optimize();
                    return *this;
                }
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
}}
