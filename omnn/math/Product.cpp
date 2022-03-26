//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Product.h"

#include "Fraction.h"
#include "Modulo.h"
#include "Infinity.h"
#include "Integer.h"
#include "Sum.h"
#include "e.h"
#include "i.h"
#include "pi.h"
#include <type_traits>
#include <boost/multiprecision/cpp_int.hpp>

namespace omnn{
namespace math {
    
    using namespace std;
    
    type_index order[] = {
        // for fast optimizing
        typeid(NaN),
        typeid(MInfinity),
        typeid(Infinity),
        typeid(Sum),
        typeid(Product),
        // general order
        typeid(Integer),
        typeid(MinusOneSq),
        typeid(Euler),
        typeid(Pi),
        typeid(Fraction),
        typeid(Exponentiation),
        typeid(Variable),
        typeid(Modulo),
    };
    
    auto ob = std::begin(order);
    auto oe = std::end(order);
    
    // inequality should cover all cases
    bool ProductOrderComparator::operator()(const Valuable& x, const Valuable& y) const
    {
        auto it1 = std::find(ob, oe, x.Type());
        if (it1==oe) IMPLEMENT

        auto it2 = std::find(ob, oe, y.Type());
        if (it2==oe) IMPLEMENT

        return it1 == it2 ? x.IsComesBefore(y) : it1 < it2;
    }
    
    Product::Product() : members{1}
    {
        hash = members.begin()->Hash();
    }
    
    Product::Product(const std::initializer_list<Valuable>& l)
    {
        for (const auto& arg : l)
        {
            if (arg.IsProduct())
                for (auto& m : arg.as<Product>().members)
                    this->Add(m, end());
            else
                this->Add(arg, end());
        }
    }
    
    int Product::findMaxVaExp()
    {
        vaExpsSum = 0;
        for (auto& i:vars) {
            if (!i.second.IsInt()) {
                IMPLEMENT;
            }
            vaExpsSum += i.second;
        }
        auto it = std::max_element(vars.begin(), vars.end(), [](auto& x, auto& y){
            return x.second < y.second;
        });
        if (it != vars.end()) {
            return static_cast<int>(it->second.as<Integer>());
        }
        return 0;
    }

    void Product::AddToVars(const Variable & va, const Valuable & exponentiation)
    {
        if (!exponentiation.IsInt()) {
            std::cerr << va.str() << '^' << exponentiation.str() << std::endl;
            IMPLEMENT // estimate in to be greater for those which you want to see first in sum sequence
        }
        if(exponentiation==0)
            return;

        auto& eai = exponentiation.as<Integer>();
        vaExpsSum += eai;
        
        auto& e = vars[va];
        if (!e.IsInt()) {
            IMPLEMENT
        }
        auto wasMax = maxVaExp == max_exp_t(e.ca(), 1);
        e += exponentiation;
      
        auto isMax = maxVaExp < e.ca();
        if (isMax) {
            maxVaExp = e.ca();
        }
        
        if (e == 0) {
            vars.erase(va);
        }
        
        if (!isMax && wasMax) {
            assert(eai < 0);
            maxVaExp = findMaxVaExp();
        }
    }

    void Product::AddToVarsIfVaOrVaExp(const Valuable &item)
    {
        if(item.IsVa())
        {
            AddToVars(item.as<Variable>(), 1_v);
        }
        else if (item.IsExponentiation())
        {
            auto& e = item.as<Exponentiation>();
            auto& base = e.getBase();
            if (base.IsVa())
            {
                AddToVars(base.as<Variable>(), e.getExponentiation());
            }
            else
            {
                auto itemMaxVaExp = item.getMaxVaExp();
                if (itemMaxVaExp > maxVaExp) {
                    maxVaExp = itemMaxVaExp;
                }
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
    
    const Product::iterator Product::Add(const Valuable& item, const iterator hint)
    {
        iterator it;
        
        if (members.size() == 1 && !item.IsSimple()) {
            it = begin();
            if (it->Same(1_v)) {
                Delete(it);
            }
        }
        
        if (item.IsInt()) {
            if (item == 1_v)
                it = begin();
            else if ((it = GetFirstOccurence<Integer>()) != end()
                || ((it = GetFirstOccurence<Fraction>()) != end() && it->IsSimpleFraction())
            ) {
                Update(it, *it * item);
            } else {
                it = base::Add(item, hint);
            }
        } else if (item.IsSimpleFraction()) {
            if ((it = GetFirstOccurence<Fraction>()) != end()
                || (it = GetFirstOccurence<Integer>()) != end()
            ) {
                Update(it, item * *it);
            } else {
                it = base::Add(item, hint);
            }
        } else if (item.IsProduct()) {
            it = hint;
            for (auto& i : item.as<Product>()) {
                it = Add(i, it);
            }
        } else if (item.IsFraction() && item.FindVa()) {
            auto& f = item.as<Fraction>();
            auto cmp = members.value_comp();
            it = std::min(Add(f.getNumerator()),
                          Add(f.getDenominator() ^ -1),
                          [&](auto& _1, auto& _2){ return cmp(*_1, *_2); });
        }
        else
        {
            it = std::find(members.begin(), members.end(), item);
            if(it==end()) {
                it = base::Add(item, hint);
                AddToVarsIfVaOrVaExp(item);
            } else
                Update(it, item.Sq());
        }
        return it;
    }

    void Product::Delete(typename cont::iterator& it)
    {
        std::function<void()> addToVars;
        if(it->IsVa())
        {
            addToVars = std::bind(&Product::AddToVars, this, it->as<Variable>(), -1_v);
        }
        else if (it->IsExponentiation())
        {
            auto& e = it->as<Exponentiation>();
            auto& ebase = e.getBase();
            if (ebase.IsVa()) {
                addToVars = std::bind(&Product::AddToVars, this, ebase.as<Variable>(), -e.getExponentiation());
            }
        }
        
        base::Delete(it);

        if(addToVars)
            addToVars();
    }

    void Product::optimize()
    {
        if (!optimizations || optimized)
            return;
        optimized = true;

        // zero
        auto it = GetFirstOccurence<Integer>();
        if (it != end()) {
            if (it->Same(0)) {
                Become(0);
                return;
            }
            
        // one
            if (it->Same(1) && size() > 1) {
                Delete(it);
            }
        }

        // fractionless
        bool updated;
        do {
            updated = {};
            for (auto it = GetFirstOccurence<Fraction>(); it != end(); ++it) {
                if (it->IsFraction() && it->FindVa()) {
                    auto& f = it->as<Fraction>();
                    auto& dn = f.getDenominator();
                    auto defract = dn.FindVa() ? (dn ^ (-1_v)) : 1_v / dn;
                    auto& n = f.getNumerator();
                    if(n==1_v)
                    {
                        Delete(it);
                    }
                    else
                    {
                        Update(it, n);
                        OptimizeOff o;
                        operator*=(defract);
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
                auto sum = std::move(const_cast<Valuable&&>(*it));
                sum.optimize();
                Delete(it);
//                auto was = optimizations;
//                optimizations = false;
                for (auto& it : members)
                {
                    sum *= it;
                }
//                optimizations = was;
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
                for (auto& m : it->as<Product>())
                    Add(m);
                Delete(it);
            }
            else
                ++it;
        }

        do
        {
            updated = {};
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
                auto it2 = it;
                ++it2;
                while (it2 != members.end())
                {
                    if (c.MultiplyIfSimplifiable(*it2)) {
                        if (c.IsProduct()) {
                            auto& cAsP = c.as<Product>();
                            if(cAsP.size() == 2 && cAsP.begin()->IsSimple()) {
                                break;
                            } else {
                                IMPLEMENT
                            }
                        }
                        Delete(it2);
                        continue;
                    }
                    else
                        ++it2;
                }

                if (!it->Same(c)) {
                    Update(it, c);
                    updated = true;
                } else {
                    ++it;
                }
            }
        } while (updated);
        
        // fraction optimizations
        auto f = GetFirstOccurence<Fraction>();
        if (f != members.end()) {
            Valuable fo = *f;
            auto& dn = fo.as<Fraction>().getDenominator();
            if (dn.IsProduct()) {
                auto& pd = dn.as<Product>();
                for (auto it = members.begin(); it != members.end();)
                {
                    if (it != f && pd.Has(*it)) {
                        fo *= *it;
                        Delete(it);
                        
                        if (!fo.IsFraction() ||
                            !fo.as<Fraction>().getDenominator().IsProduct()
                            ) {
                            break;
                        }
                    }
                    else  ++it;
                }
            }
            
            fo.optimize();
            
            if (fo.IsFraction()) {
                auto& dn = fo.as<Fraction>().getDenominator();
                if (!dn.IsProduct()) {
                    for (auto it = members.begin(); it != members.end();)
                    {
                        if (dn == *it) {
                            Delete(it);
                            fo *= dn;
                            break;
                        } else if (it->IsExponentiation() && it->as<Exponentiation>().getBase() == dn) {
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
            Become(std::move(const_cast<Valuable&&>(*members.begin())));
    }

    Valuable Product::Sqrt() const
    {
        return Each([](auto& m){ return m.Sqrt(); });
    }

    Valuable & Product::sq()
    {
        Product p;
        for (auto m : members)
        {
            p.Add(m.Sq());
        }
        return Become(std::move(p));
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
                auto& i1 = kv.second.ca();
                auto& i2 = it->second.ca();
                if (i1 > 0_v && i2 > 0_v) {
                    common[kv.first] = std::min(i1, i2);
                }
                else
                {
                    IMPLEMENT
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
                if(c.IsInt()) {
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

    Valuable Product::InCommonWith(const Valuable& v) const
    {
        auto _ = 1_v;
        auto check = [&](auto& with){
            if (std::find(members.begin(), members.end(), with) != members.end())
                _ *= with;
            else
                for(auto&m:members){
                    auto c = m.InCommonWith(with);
                    if (c!=1_v)
                        _*=c;
                }
        };

        if (v.IsProduct())
            for(auto& m : v.as<Product>())
                check(m);
        else if (v.IsSum())
            _ = v.InCommonWith(*this);
        else
            check(v);
        return _;
    }

    // NOTE : inequality must cover all cases for bugless Sum::Add
    bool Product::IsComesBefore(const Valuable& v) const
    {
        auto mae = getMaxVaExp();
        auto vme = v.getMaxVaExp();
        
        if (mae != vme)
            return mae > vme;
        
        char vp[sizeof(Product)];
        auto isSameType = v.IsProduct();
        if(!isSameType)
        {
            static const ProductOrderComparator oc;
            return oc(*this,v);
        }
        auto p = isSameType ? &v.as<Product>() : new(vp) Product{v};
        auto d = [isSameType](const Product* _){
            if (!isSameType && _){
                _->~Product();
            }
        };
        std::unique_ptr<const Product , decltype(d)> up(p,d);
        if (p)
        {
            if (vme != p->getMaxVaExp()) {
                std::cout << vme << std::endl;
                std::cout << p->getMaxVaExp() << std::endl;
                IMPLEMENT
            }

            if (vaExpsSum.ca() != p->vaExpsSum.ca())
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
                auto it1 = std::find(ob, oe, i1->Type());
                assert(it1!=oe); // IMPLEMENT, add to order table
                auto it2 = std::find(ob, oe, i2->Type());
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
    
    Valuable& Product::operator +=(const Valuable& v)
    {
        if(v == 0_v)
            return *this;
        if (*this == v)
            return *this *= 2;
        if(*this == -v)
            return Become(0_v);
        if(v.IsProduct()){
            auto& vAsP = v.as<Product>();
            Product thisHasNotCommonWithV, vHasNotInCommonWithThis;
            Product common;
            for(auto&& item : GetCont()){
                if(vAsP.Has(item))
                    common.Add(std::move(item));
                else
                    thisHasNotCommonWithV.Add(std::move(item));
            }
            for(auto& item : vAsP.GetConstCont()){
                if(!common.Has(item))
                    vHasNotInCommonWithThis.Add(item);
            }
            if(!(common.size()==1 && *common.begin()==1)){ // unchanged
                Valuable sum = Sum {thisHasNotCommonWithV, vHasNotInCommonWithThis};
                sum.optimize();
                sum *= common;
                sum.optimize();
                return Become(std::move(sum));
            }
        } else if (v.IsSum()) {
            return Become(v + *this);
        }
        else{
        auto& cv = getCommonVars();
        if (!cv.empty() && cv == v.getCommonVars())
        {
            auto valuable = varless() + v.varless();
            if(!valuable.IsSum())
                valuable *= getVaVal();
            return Become(std::move(valuable));
        }
        }
        return Become(Sum { *this, v });
    }

    std::pair<Valuable, Valuable> Product::SplitSimplePart() const {
        std::pair<Valuable, Valuable> parts;
        auto it = begin();
        IMPLEMENT
    }

    std::pair<Valuable, Valuable> Product::split_simple_part(){
        IMPLEMENT

    }

   std::pair<bool,Valuable> Product::IsSummationSimplifiable(const Valuable& v) const
   {
       std::pair<bool,Valuable> is;
       is.first = v == 0;
       if (is.first)
           is.second = *this;
       else if ((is.first = operator==(v)))
           is.second = *this * 2;
       else if ((is.first = operator==(-v)))
           is.second = 0;
       else if (Has(v)) {
           //OptimizeOn o;
           auto div = *this / v;
           auto divPlusOneIsSimple = div.IsSummationSimplifiable(vo<1>::get());
           is.first = divPlusOneIsSimple.first;
           if(divPlusOneIsSimple.first) {
               is.second = divPlusOneIsSimple.second * v;
           }
       } else if (v.IsExponentiation()) {
           is.first = Has(v) && size() == 2 && begin()->IsSimple();
           if(is.first) {
               is.second++ = *this;
               is.first = !is.second.IsSum();
           }
       } else if (v.IsSimple()) {
       } else if (v.IsProduct()
                  || v.IsVa()
                  || v.IsFraction()
                  ) {
           //OptimizeOn o;
           auto icw = InCommonWith(v);
           if (icw != 1) {
               auto thisNoCommon = *this / icw;
               if (!operator==(thisNoCommon)) { //multivalue cases (example ((-17)/16)*(4^((1/2))) / (1/2)*(4^((1/2))) = ((-17)/16)*(4^((1/2)))
                   auto vNoCommon = v / icw;
                   //std::cout << thisNoCommon << ".IsSummationSimplifiable(" << vNoCommon << ')' << std::endl;
                   is = thisNoCommon.IsSummationSimplifiable(vNoCommon);
                   if(is.first){
                       is.second *= icw;
                   }
               }
           }
//           auto& vp = v.as<Product>();
//           auto sp = SplitSimplePart();
//           auto vsp = vp.SplitSimplePart();
//           if(sp.second == vsp.second){
//
//               IMPLEMENT
//           }
       } else {
//           std::cout << v <<std::endl;
           LOG_AND_IMPLEMENT("Unknown case (need to implement) in Product::IsSummationSimplifiable for " << *this << " and " << v);
       }
       return is;
   }

    Valuable& Product::operator *=(const Valuable& v)
    {
        if ((size() == 1 && *begin() == 1) || size() == 0)
            return Become(Valuable(v));

        if (v.IsInt()){
            if (v==0) {
                return Become(0);
            } else if (v==1) {
                goto yes;
            } else {
                auto b = begin();
                if(b->IsSimple()){
                    auto up = *b * v;
                    if(up == 1)
                        Delete(b);
                    else
                        Update(b, up);
                    goto yes;
                }
            }
        }

        if (v.IsSum())
            return Become(v**this);

        if (v.IsVa())
        {
            auto& va = v.as<Variable>();
            for (auto it = members.begin(); it != members.end(); ++it)
            {
                if (it->Same(v)) {
                    Update(it, Exponentiation(va, 2));
                    goto yes;
                }
                else if (it->IsExponentiation())
                {
                    auto& e = it->as<Exponentiation>();
                    if (e.getBase() == va) {
                        Update(it, va ^ (e.getExponentiation()+1));
                        goto yes;
                    }
                }
            }
        }
        else if (v.IsExponentiation())
        {
            auto& e = v.as<Exponentiation>();
            auto& vExpBase = e.getBase();
            for (auto it = members.begin(); it != members.end();)
            {
                if (it->IsExponentiation() &&
                    it->as<Exponentiation>().getBase() == vExpBase)
                {
                    Update(it, *it*v);
                    goto yes;
                }
                else
                {
                    if (vExpBase == *it) {
                        Update(it, vExpBase ^ (e.getExponentiation()+1));
                        goto yes;
                    }
                    else
                        ++it;
                }
            }
        }
        else if (v.IsProduct())
        {
            if(operator==(v))
                sq();
            else
                for(auto& m : v.as<Product>())
                    base::Add(m);
            goto yes;
        }
        else
        {
            for (auto it = members.begin(); it != members.end();)
            {
                if (it->OfSameType(v)) {
                    auto up = *it * v;
                    if(up == 1){
                        Delete(it);
                    } else {
                        Update(it, up);
                    }
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
        if (v.IsProduct()) {
            auto opts = optimizations;
            optimizations = {};
            for(auto& i : v.as<Product>())
                *this /= i;
            optimizations = opts;
            optimize();
            return *this;
        }
        else if (v.IsSimple()) {
            if (v == 1_v) {
                return *this;
            }
            auto first = begin();
            if (first->IsSimple()) {
                auto _ = (*first) / v;
                if (_.IsInt() && _.ca() == 1)
                    Delete(first);
                else {
                    Update(first, _);
                    optimized = {};
                }
                optimize();
                return *this;
            }
            else {
                return operator *=(Fraction{ 1_v,v });
            }
        }
        else
        {
            if (v.IsMultival() != YesNoMaybe::Yes) {
                auto it = std::find(begin(), end(), v);
                if (it != end()) {
                    Delete(it); // TODO : Review this branch to make it compatible with multivalue v
                    optimize();
                    return *this;
                }
            }
            auto vIsExp = v.IsExponentiation();
            auto e = vIsExp ? &v.as<Exponentiation>() : nullptr;
            for (auto it = members.begin(); it != members.end(); ++it)
            {
                if (*it == v)
                {
                    if (v.IsMultival() == YesNoMaybe::No) {
                        Delete(it);
                        optimize();
                        return *this;
//                    } else if (v.IsMultival() == YesNoMaybe::Maybe) {
                        
                    } else {
                        Update(it, *it * (v^-1));
                        optimize();
                        return *this;
                    }
                }
                else if (vIsExp && *it == e->getBase())
                {
                    return *this *= e->getBase() ^ (-e->getExponentiation());
                }
                else if (it->IsExponentiation() && it->as<Exponentiation>().getBase() == v)
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

	namespace {
        constexpr std::hash<a_int> Hasher;
        const size_t Hash1 = Hasher(1);
    }
    bool Product::operator ==(const Valuable& v) const
    {
        auto sameHash = (Valuable::hash & ~Hash1) == (v.Hash() & ~Hash1); // ignore multiplication by 1
        auto same = v.Is<Product>() && sameHash;
        auto& c1 = GetConstCont();
        auto sz1 = c1.size();
        if (same) {
            auto& vp = v.as<Product>();
            auto& c2 = vp.GetConstCont();
            auto sz2 = c2.size();
            auto sameSizes = sz1 == sz2;
            if(sameSizes) {
                same = c1 == c2;                
            } else if (sz1-sz2==1 || sz2-sz1==1) {
                auto it1 = c1.begin(), it2 = c2.begin();
                auto e1 = c1.end(), e2 = c2.end();                    
                for(same = true;
                    same && it1 != e1 && it2 != e2;
                    ++it1, ++it2)
                {
                    if(it1->Same(1_v)){
                        ++it1;
                    }
                    if(it2->Same(1_v)){
                        ++it2;
                    }
                    
                    if(it1 == e1 || it2 == e2)
                        continue;
                    
                    same = same && it1->Same(*it2);
                }
                
                same = same && it1 == e1 && it2 == e2;
            } else {
                same = {};
            }
        }
        else if (sameHash
                 && (sz1 == 1
                     || (sz1 == 2 && c1.begin()->Same(1_v) )))
        {
            same = c1.rbegin()->operator==(v);
        }
        return same;
    }

    Product::operator double() const
    {
        double d=1;
        for(auto& i:members)
        {
            d*=static_cast<double>(i);
        }
        return d;
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
    
    Valuable Product::operator()(const Variable& va) const
    {
        return operator()(va, 0_v);
    }
    
    Valuable Product::operator()(const Variable& va, const Valuable& augmentation) const
    {
        Valuable s; s.SetView(Valuable::View::Flat);
       
        if(augmentation.HasVa(va)) {
            IMPLEMENT;
        } else {
            auto coVa = getCommonVars();
            auto it = coVa.find(va);
            if (it != coVa.end()) {
                if (it->second < 0) {
                    s = ((*this / (it->first ^ it->second)) / augmentation) ^ (1_v / -it->second);
                }
                else
                {
                    s = (augmentation / (*this / (it->first ^ it->second))) ^ (1_v / it->second);
                }
            }
            else
            {
                auto a = 1_v;
                auto aug = augmentation;
                for(auto& m : members)
                    if (m.HasVa(va))
                        a *= m;
                    else
                        aug *= m;
                if (a==1) {
                    IMPLEMENT
                }
                s = a(va,aug);
            }
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
    
    void Product::solve(const Variable& va, solutions_t& solutions) const
    {
        auto it = std::find(members.begin(), members.end(), va);
        if(it != members.end())
            solutions.insert(0_v);
        else {
            it = begin();
            bool found = {};
            const Exponentiation* e = {};
            while (it != end()
                    && !(it->IsExponentiation()
                     && (found = it->as<Exponentiation>().getBase() == va)
                     ) ) {
                ++it;
            }
            if (found) {
                if (e->getExponentiation() == 0_v) {
                    IMPLEMENT
                }
                solutions.insert(0_v);
            }
            else {
                std::cout << "Solving " << str() << std::endl;
                IMPLEMENT // TODO: find exponentiations of va
            }
        }
    }

    Valuable::solutions_t Product::Distinct() const {
        solutions_t branches = { 1_v };
        for (auto& m : members) {
            solutions_t newBranches;
            for (auto&& branch : m.Distinct()) {
                for (auto& b : branches) {
                    newBranches.emplace(b * branch);
                }
            }
            branches = std::move(newBranches);
        }
        return branches;
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
