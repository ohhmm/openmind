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
        return it1 == it2 ? x.IsComesBefore(y) : it1 < it2;
    }
    
    Product::Product() : members {{1}}
    {
        hash = members.begin()->Hash();
    }
    
    Product::Product(const std::initializer_list<Valuable>& l)
    {
        for (const auto& arg : l)
        {
            auto a = cast(arg);
            if(a)
                for(auto& m: *a)
                    this->Add(m);
            else
                this->Add(arg);
        }
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
        if(item.IsVa())
        {
            auto v = Variable::cast(item);
            AddToVars(*v, 1_v);
        }
        else if (item.IsExponentiation())
        {
            auto e = Exponentiation::cast(item);
            auto isVa = e->getBase().IsVa();
            if (isVa)
            {
                auto v = Variable::cast(e->getBase());
                AddToVars(*v, e->getExponentiation());
            }
        }
        else if (!item.IsSum() && item.FindVa())
        {
            IMPLEMENT
        }
    }
    
    Product::iterator Product::Had(iterator it)
    {
        hash ^= it->Hash();
        AddToVarsIfVaOrVaExp(*it);
        base::Update(it, *it ^ 2);
        return it;
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

    void Product::optimize()
    {
        if (!optimizations) return;
        
        // fractionless
        bool updated;
        do {
            updated = {};
            for (auto it = GetFirstOccurence<Fraction>(); it != end(); ++it) {
                if (it->IsFraction() && it->FindVa()) {
                    auto f=Fraction::cast(*it);
                    auto dn = f->getDenominator();
                    auto defract = dn.FindVa() ? (dn ^ (-1_v)) : 1_v / dn;
                    auto n = f->getNumerator();
                    if(n==1_v)
                    {
                        Delete(it);
                    }
                    else
                    {
                        Update(it, n);
                        auto o = optimizations;
                        optimizations = false;
                        operator*=(defract);
                        optimizations = o;
                    }
                    updated = true;
                }
            }
        } while (updated);

        
        // optimize members, if found a sum then become the sum multiplied by other members
        for (auto it = members.begin(); it != members.end();)
        {
            if (it->IsSum())
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
        
        // emerge inner products
        for (auto it = members.begin(); it != members.end();)
        {
            if (it->IsProduct()) {
                for (auto& m : *cast(*it))
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
            Valuable fo = *f;
            auto fc = Fraction::cast(fo);
            auto dn = fc->getDenominator();
            auto pd = Product::cast(dn);
            if (pd) {
                for (auto it = members.begin(); it != members.end();)
                {
                    if (pd->Has(*it)) {
                        fo *= *it;
                        Delete(it);
                        
                        if ((fc = Fraction::cast(fo)) &&
                            (dn = fc->getDenominator()) &&
                            (pd = Product::cast(dn))
                            ) {
                            it = pd->begin();
                        }
                        else
                            break;
                    }
                    else  ++it;
                }
            }
            
            fo.optimize();
            
            if (fo.IsFraction()) {
                auto dn = Fraction::cast(fo)->getDenominator();
                if (!dn.IsProduct()) {
                    for (auto it = members.begin(); it != members.end();)
                    {
                        if (dn == *it) {
                            Delete(it);
                            fo *= dn;
                            break;
                        } else if (it->IsExponentiation() && Exponentiation::cast(*it)->getBase()==dn) {
                            Update(it, *it/dn);
                            fo *= dn;
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
            auto p = cast(v);
            Product vp{v};
            if(!p)
            {
                p=cast(vp);
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
        if(v == 0_v)
            return *this;
        if (*this == v)
            return *this *= 2;
        if(*this == -v)
            return Become(0_v);
        
        auto cv = getCommonVars();
        if (!cv.empty() && cv == v.getCommonVars())
        {
            auto valuable = varless() + v.varless();
            if(!valuable.IsSum())
                valuable *= getVaVal();
            return Become(std::move(valuable));
        }

        return Become(Sum { *this, v });
    }

    Valuable& Product::operator *=(const Valuable& v)
    {
        auto s = Sum::cast(v);
        if (s)
            return Become(v**this);

        if (v.IsVa())
        {
            auto va = Variable::cast(v);
            for (auto it = members.begin(); it != members.end(); ++it)
            {
                if (it->Same(v)) {
                    Update(it, Exponentiation(*va, 2));
                    goto yes;
                }
                else if (it->IsExponentiation())
                {
                    auto e = Exponentiation::cast(*it);
                    if (e && e->getBase() == *va) {
                        Update(it, Exponentiation(*va, e->getExponentiation()+1));
                        goto yes;
                    }
                }
            }
        }
        else if (v.IsExponentiation())
        {
            auto e = Exponentiation::cast(v);
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
        else if (v.IsProduct())
        {
            for(auto& m : *cast(v))
            {
                Add(m);
            }
            goto yes;
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
        if (v.IsProduct()) {
            auto p = Product::cast(v);
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
                else if (it->IsExponentiation() && Exponentiation::cast(*it)->getBase()==v)
                {
                    Update(it, *it / v);
                    optimize();
                    return *this;
                }
            }
        }
//        else if (v.IsVa())
//        {
//            auto it = getCommonVars().find(v);
//            if (it != getCommonVars().end() && *it != 0_v) {
//                if (*it == 1_v) {
//                    auto _ = find(begin(), end(), v);
//                    if (_ == end()) {
//                        IMPLEMENT
//                    }
//                    else
//                    {
//                        Delete(_);
//                    }
//                } else {
//                    for(auto i = begin(); i != end(); ++i)
//                    {
//                        if (i->IsExponentiation()) {
//                            auto e = Exponentiation::cast(*i);
//                            if (e->getBase() == v) {
//                                Update(i, *e / v);
//                                optimize();
//                                break;
//                            }
//                        }
//                    }
//                }
//            }
//        }
        return *this *= v ^ -1;
	}

    Valuable& Product::operator ^=(const Valuable& v)
    {
        auto _ = 1_v;
        for(auto& m : members)
        {
            _ *= m ^ v;
        }
        Become(std::move(_));
        return *this;
    }

	Valuable& Product::operator %=(const Valuable& v)
	{
		return base::operator %=(v);
	}

	Valuable& Product::operator --()
	{
        return operator+=(-1);
	}

	Valuable& Product::operator ++()
	{
		return operator+=(1);
	}

    Valuable& Product::d(const Variable& x)
    {
        if(vars.find(x) != vars.end())
        {
            *this *= vars[x];
            *this /= x;
            optimize();
        }else
            Become(0_v);
        return *this;
    }
    
    Valuable::solutions_t Product::operator()(const Variable& va) const
    {
        return operator()(va, 0_v);
    }
    
    Valuable::solutions_t Product::operator()(const Variable& va, const Valuable& augmentation) const
    {
        Valuable::solutions_t s;
       
        if(augmentation.HasVa(va)) {
            IMPLEMENT;
        } else {
            auto coVa = getCommonVars();
            auto it = coVa.find(va);
            if (it != coVa.end()) {
                if (it->second < 0) {
                    s.insert(((*this / (it->first ^ it->second)) / augmentation) ^ (1_v / -it->second));
                }
                else
                {
                    s.insert((augmentation / (*this / (it->first ^ it->second))) ^ (1_v / it->second));
                }
            }
            else
                IMPLEMENT
        }
        
//        if(augmentation.HasVa(va)) {
//            IMPLEMENT;
//        } else {
//            auto _ = augmentation;
//            auto left = 1_v;
//            for(auto& m : members)
//            {
//                if (m.HasVa(va)) {
//                    left *= m;
//                } else {
//                    _ /= m;
//                }
//            }
//            
//            left.optimize();
//            if (left.IsProduct()) {
//                IMPLEMENT
//            }
//            
//            return left(va, _);
//        }
//        auto cova = getCommonVars();
//        auto it = cova.find(va);
//        if (it != cova.end()) {
//            auto _ = augmentation / (*this / (va ^ it->second));
//            if(_.HasVa(va))
//                IMPLEMENT;
//            s.insert(_);
//            if (it->second % 2 == 0) {
//                s.insert(-_);
//            }
//        }
//        else
//        {
//            IMPLEMENT
//        }
        return s;
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
