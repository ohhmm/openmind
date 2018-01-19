//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Product.h"

#include "Fraction.h"
#include "Integer.h"
#include "Sum.h"

#include <type_traits>

namespace omnn{
namespace math {
    
    using namespace std;
    
    type_index order[] = {
        // for fast optimizing
        typeid(Sum),
        typeid(Product),
        // general order
        typeid(Integer),
        typeid(Fraction),
        typeid(Exponentiation),
        typeid(Variable),
    };
    
    auto ob = std::begin(order);
    auto oe = std::end(order);
    
    // inequality should cover all cases
    bool ProductOrderComparator::operator()(const Valuable& x, const Valuable& y) const
    {
        auto it1 = std::find(ob, oe, static_cast<type_index>(x));
        assert(it1!=oe); // IMPLEMENT
        auto it2 = std::find(ob, oe, static_cast<type_index>(y));
        assert(it2!=oe); // IMPLEMENT
        return it1 == it2 ? *it1 > *it2 : it1 < it2;
    }
    
    int Product::findMaxVaExp()
    {
        for (auto& i:vars) {
            vaExpsSum += static_cast<int>(i.second);
        }
        auto it = std::max_element(vars.begin(), vars.end(), [](auto& x, auto& y){
            return x.second < y.second;
        });
        if (it != vars.end()) {
            auto i = Integer::cast(it->second);
            return static_cast<int>(*i);
        }
        return 0;
    }

    void Product::AddToVars(const Variable & item, const Valuable & exponentiation)
    {
        vars[item] += exponentiation;
        if (vars[item] == 0) {
            vars.erase(item);
            maxVaExp = findMaxVaExp();
        }
        auto i = Integer::cast(exponentiation);
        if (i) {
            int in = static_cast<decltype(vaExpsSum)>(*i); // <<< this in
            vaExpsSum += in;
            if (in > getMaxVaExp()) {
                maxVaExp = in;
            }
        }
        else
            IMPLEMENT // just estimate in to be greater for those which you want to see first in sum sequence
    }

    void Product::AddToVarsIfVaOrVaExp(const Valuable &item)
    {
        auto v = Variable::cast(item);
        if(v)
        {
            AddToVars(*v, 1_v);
        }
        else
        {
            auto e = Exponentiation::cast(item);
            if (e)
            {
                v = Variable::cast(e->getBase());
                if (v)
                {
                    AddToVars(*v, e->getExponentiation());
                }
            }
        }
    }

    const Product::iterator Product::Add(const Valuable& item)
    {
        auto it=base::Add(item);
        AddToVarsIfVaOrVaExp(item);
        return it;
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
            AddToVars(*v, -1_v);
        }
        else
        {
            auto e = Exponentiation::cast(*it);
            if (e)
            {
                v = Variable::cast(e->getBase());
                if (v) {
                    AddToVars(*v, -e->getExponentiation());
                }
            }
        }
        
        base::Delete(it);
    }
    
	Valuable Product::operator -() const
	{
        Valuable v = *this;
        auto p = cast(v);
        auto b = p->GetFirstOccurence<Integer>();
        if (b == p->end()) {
            b = p->GetFirstOccurence<Fraction>();
            if (b == p->end()) {
                b = p->begin();
            }
        }
        const_cast<Product*>(p)->Update(b, -*b);
        v.optimize();
        return v;
	}

    void Product::optimize()
    {
        //if (!optimizations) return;
        
        // optimize members, if found a sum then become the sum multiplied by other members
        for (auto it = members.begin(); it != members.end();)
        {
            auto s = Sum::cast(*it);
            if (s)
            {
                auto sum = std::move(*it);
                Delete(it);
                auto was = optimizations;
                //optimizations = false;
                for (auto& it : members)
                {
                    sum *= it;
                }
                optimizations = was;
                Become(std::move(sum));
                return;
            }
            auto c = *it;
            c.optimize();
            if (!it->Same(c)) {
                Update(it, c);
                continue;
            }
            else
                ++it;
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
                    continue;
                }
                else if (it->Same(*it2) && v)
                {
                    c ^= 2;
                    Delete(it2);
                    e = Exponentiation::cast(c);
                    f = Fraction::cast(c);
                    i = Integer::cast(c);
                    v = Variable::cast(c);
                    continue;
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
            auto dn = const_cast<Fraction*>(Fraction::cast(fo))->Denominator();
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
        else if (members.size()==1)
            Become(std::move(*const_cast<Valuable*>(&*members.begin())));
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
                if (i1 && i2 && *i1 > 0_v && *i2 > 0_v) {
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
    
    Valuable Product::calcFreeMember() const
    {
        Valuable _ = 1_v;
        if (getCommonVars().empty()) {
            for(auto& m : *this) {
                auto c = m.calcFreeMember();
                if(Integer::cast(c)) {
                    _ *= c;
                } else
                    IMPLEMENT
            }
        } else
            _ = 0_v;
        return _;
    }
    
    Valuable Product::getCommVal(const Product& with) const
    {
        return VaVal(getCommonVars(with.getCommonVars()));
    }

    // NOTE : inequality must cover all cases for bugless Sum::Add
    bool Product::IsComesBefore(const Valuable& v) const
    {
        auto is = getMaxVaExp() > v.getMaxVaExp();
        if (!is)
        {
            Valuable t;
            auto p = cast(v);
            if(!p)
            {
                t=Product(1,v);
                p=cast(t);
            }
            if (p)
            {
                if (getMaxVaExp() != p->getMaxVaExp())
                {
                    return getMaxVaExp() > p->getMaxVaExp();
                }

                if (vaExpsSum != p->vaExpsSum)
                {
                    return vaExpsSum > p->vaExpsSum;
                }

                if (members == p->members)
                    return false;

                if (members.size() != p->members.size()) {
                    return members.size() > p->members.size();
                }

                auto i1 = members.begin();
                auto i2 = p->members.begin();
                for (; i1 != members.end(); ++i1, ++i2) {
                    auto it1 = std::find(ob, oe, static_cast<type_index>(*i1));
                    assert(it1!=oe); // IMPLEMENT, add to order table
                    auto it2 = std::find(ob, oe, static_cast<type_index>(*i2));
                    assert(it2!=oe); // IMPLEMENT
                    if (it1 != it2) {
                        return it1 < it2;
                    }
                }
                // same types set, compare by value
                i1 = members.begin();
                i2 = p->members.begin();
                for (; i1 != members.end(); ++i1, ++i2) {
                    if(*i1 != *i2)
                    {
                        return i1->IsComesBefore(*i2);
                    }
                }

                // everything is equal, should not be so
                IMPLEMENT
            }
            else
                IMPLEMENT
        }
        return is;
    }
    
    Valuable& Product::operator +=(const Valuable& v)
    {
            if (*this == v)
                return *this *= 2;
            if(*this == -v)
                return Become(0_v);

            auto cv = getCommonVars();
            if (!cv.empty() && cv == v.getCommonVars())
            {
                auto valuable = varless() + v.varless();
                valuable *= getVaVal();
                return Become(std::move(valuable));
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

    Valuable& Product::d(const Variable& x)
    {
        *this *= vars[x];
        *this /= x;
        optimize();
        return *this;
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
