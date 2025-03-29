//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Product.h"

#include "Fraction.h"
#include "Modulo.h"
#include "Infinity.h"
#include "Integer.h"
#include "Sum.h"
#include "Euler.h"
#include "i.h"
#include "NaN.h"
#include "pi.h"
#include "PrincipalSurd.h"
#include "VarHost.h"

#include "omnn/rt/antiloop.hpp"
#include "omnn/rt/each.hpp"
#include "omnn/rt/find.hpp"

#include <algorithm>
#include <type_traits>
#include <ranges>

#include <boost/multiprecision/cpp_int.hpp>


using namespace omnn::math;
    
    
    constexpr auto ValueOrderComparator = [](const Valuable& x, const Valuable& y)
    {
        auto it1 = ProductOrderComparator::Order(x);
        auto it2 = ProductOrderComparator::Order(y);
        return it1 == it2 ? x > y : it1 < it2;
    };

    static constexpr ProductOrderComparator poc;

    Product::Product(const std::initializer_list<Valuable> l)
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
    
    max_exp_t Product::findMaxVaExp()
    {
        vaExpsSum = 0;
        for (auto& i:vars) {
            auto& e = i.second;
            if (e.IsInt()) {
                vaExpsSum += e.as<Integer>().ca();
            } else if (e.IsSimpleFraction()) {
                auto& f = e.as<Fraction>();
				vaExpsSum += f.getNumerator().ca() / f.getDenominator().ca();
            }
            else {
                IMPLEMENT;
            }
        }
        auto it = std::max_element(vars.begin(), vars.end(), [](auto& x, auto& y){
            return x.second < y.second;
        });
        if (it != vars.end()) {
            return static_cast<max_exp_t>(it->second);
        }
        return 0;
    }

    void Product::AddToVars(const Variable & va, const Valuable & exponentiation)
    {
        if (exponentiation.IsZero())
            return;

        if (exponentiation.IsInt()) {
            vaExpsSum += exponentiation.as<Integer>().ca();
        } else if (exponentiation.IsSimpleFraction()) {
            vaExpsSum += static_cast<decltype(vaExpsSum)>(exponentiation);
        } else if (!exponentiation.is_optimized()) {
            AddToVars(va, exponentiation.Optimized());
            return;
        } else {
            LOG_AND_IMPLEMENT("estimate in to be greater for those which you want to see first in product sequence:"
                              << va.str() << " ^ " << exponentiation.str());
        }
        
        auto& e = vars[va];
        auto re = static_cast<a_rational>(e);
        auto wasMax = maxVaExp == re;
        e += exponentiation;
        re = static_cast<a_rational>(e);
        auto isMax = maxVaExp < re;
        if (isMax) {
            maxVaExp = re;
        }
        
        if (e.IsZero()) {
            vars.erase(va);
        }
        
        if (!isMax && wasMax) {
            assert(exponentiation < 0);
            maxVaExp = findMaxVaExp();
        }
    }

    void Product::AddToVarsIfVaOrVaExp(const Valuable::vars_cont_t& info)
    {
        for (auto& kv : info)
            AddToVars(kv.first, kv.second);
    }

    void Product::AddToVarsIfVaOrVaExp(const Valuable &item)
    {
        for (auto& kv :
            //item.getCommonVars()
            item.GetVaExps()
            )
        {
            AddToVars(kv.first, kv.second);
        }
    }
    
    Product::iterator Product::Had(iterator it)
    {
        hash ^= it->Hash();
        AddToVarsIfVaOrVaExp(*it);
        base::Update(it, *it ^ 2);
        return it;
    }
    
    bool Product::VarSurdFactor(const Valuable& v)
    {
        return Sum::VarSurdFactor(v);
	}

    const Product::iterator Product::Add(const Valuable& item, const iterator hint) {
        auto copy = item;
        return Add(std::move(copy), hint);
    }

    const Product::iterator Product::Add(Valuable&& item, const iterator hint)
    {
        iterator it;
        
        if (members.size() == 1 && !item.IsSimple()) {
            it = begin();
            if (it->Same(constants::one)) {
                Delete(it);
            }
        }
        
        if (item.IsInt()) {
            if (item == constants::one)
                it = begin();
            else if ((it = GetFirstOccurence<Integer>()) != end()
                || ((it = GetFirstOccurence<Fraction>()) != end() && it->IsSimpleFraction())
            ) {
                Update(it, *it * item);
            } else {
                it = base::Add(std::move(item), hint);
            }
        } else if (item.IsSimpleFraction()) {
            if ((it = GetFirstOccurence<Fraction>()) != end()
                || (it = GetFirstOccurence<Integer>()) != end()
            ) {
                Update(it, item * *it);
            } else {
                it = base::Add(std::move(item), hint);
            }
        } else if (item.IsProduct()) {
            it = hint;
            auto& p = item.as<Product>();
            for (auto i = p.size(); i-->0 ;)
            {
                it = Add(std::move(p.Extract()), it);
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
                auto info = item.GetVaExps();
                it = base::Add(std::move(item), hint);
                AddToVarsIfVaOrVaExp(info);
            } else {
                Update(it, std::move(item.sq()));
            }
        }
        return it;
    }

    void Product::Delete(typename cont::iterator& it)
    {
        std::function<void()> addToVars;
        if(it->IsVa())
        {
            addToVars = std::bind(&Product::AddToVars, this, it->as<Variable>(), constants::minus_1);
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

    void Product::Update(iterator& it, Valuable&& value) {
        if (value == constants::one) {
            Delete(it);
        } else {
            base::Update(it, std::move(value));
        }
    }

    void Product::Update(iterator& it, const Valuable& value) {
        if (value == constants::one) {
            Delete(it);
        } else {
            base::Update(it, value);
        }
    }

    void Product::optimize()
    {
        if (!optimizations || optimized)
            return;
        MarkAsOptimized();
        ANTILOOP(Product)

        auto isPlusMinus = YesNoMaybe::Maybe;
        auto IsPlusMinus = [&] {
            if (isPlusMinus == YesNoMaybe::Maybe) {
                if (Has(constants::plus_minus_1))
                    isPlusMinus = YesNoMaybe::Yes;
            }
            return isPlusMinus == YesNoMaybe::Yes;
        };
        auto TryBePositive = [&](auto& it) {
            if (it->IsSimple() && *it < 0 && IsPlusMinus()) {
                Update(it, -*it);
            }
        };

        // NaN
        auto it = GetFirstOccurence<NaN>();
        if (it != end()) {
            Become(Extract(it));
            return;
        }

        // zero
        it = GetFirstOccurence<Integer>();
        if (it != end()) {
            if (it->IsZero()) {
                Become(Extract(it));
                return;
            } else if (it->Same(constants::one) && size() > 1) {
                Delete(it);
            }

			TryBePositive(it);
        }

        if (IsEquation()) {
            for (auto it = members.begin(); it != members.end();) {
                if (!it->FindVa()) {
                    Delete(it);
                } else
                    ++it;
            }
        }

        // fractionless
        bool updated;
        do {
            updated = {};
            for (auto it = GetFirstOccurence<Fraction>(); it != end(); ++it) {
                if (it->IsFraction()) {
                    TryBePositive(it);
                    updated = it == end();
                    if (updated) {
                        break;
                    }
                    if (it->FindVa()) {
                        auto& f = it->as<Fraction>();
                        auto& dn = f.getDenominator();
                        auto defract = dn.FindVa() ? (dn ^ constants::minus_1) : dn.Reciprocal();
                        auto& n = f.getNumerator();
                        if (n == constants::one) {
                            Delete(it);
                        } else {
                            Update(it, n);
                            OptimizeOff o;
                            operator*=(defract);
                        }
                        updated = true;
                    }
                }
            }
        } while (updated);


        // optimize members, if found a sum then become the sum multiplied by other members
        for (auto it = members.begin(); it != members.end();)
        {
            if (it->IsSum())
            {
                auto sum = std::move(members.extract(it).value());
                sum.optimize();
                for (auto& m : members)
                {
                    sum *= m;
                }
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

        const Valuable was(Clone());
        do
        {
            updated = {};
            for (auto it = members.begin(); it != members.end();)
            {
                if (members.size() == 1)
                {
                    auto node = members.extract(it);
                    Become(std::move(node.value()));
                    return;
                }

                if (*it == 1)
                {
                    Delete(it);
                    continue;
                }

                if (it->IsZero())
                {
                    Become(Extract(it));
                    return;
                }

                auto c = *it;
                auto it2 = it;
                ++it2;
                while (it2 != members.end())
                {
                    auto simplifiable = it2->IsMultiplicationSimplifiable(c);
                    if (simplifiable.first) {
                        Delete(it2);
                        std::swap(c, simplifiable.second);
                        if (c.IsProduct()) {
                            auto& cAsP = c.as<Product>();
                            if(cAsP.size() == 2 && cAsP.begin()->IsSimple()) {
                                break;
                            } else {
                                IMPLEMENT
                            }
                        }
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
        } while (updated && !Same(was));

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
            Become(std::move(members.extract(members.begin()).value()));
    }

    Valuable Product::Sqrt() const
    {
        return Each([](auto& m){ return m.Sqrt(); });
    }

    Valuable & Product::sq()
    {
        Product p({});
        for (auto m : members)
        {
            p.Add(m.Sq());
        }
        return Become(std::move(p));
    }

    Valuable Product::abs() const
    {
        auto val = Each([](auto& m) { return std::abs(m); });
        val.optimize();
        return val;
    }

    Valuable& Product::reciprocal() {
        Product p({});
        for (auto i = size(); i-->0 ;)
        {
            auto m = Extract();
            p.Add(std::move(m.reciprocal()));
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
                if (i1 > constants::zero && i2 > constants::zero) {
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
            _ = constants::zero;
        return _;
    }
    
    Valuable Product::getCommVal(const Product& with) const
    {
        return VaVal(getCommonVars(with.getCommonVars()));
    }

    bool Product::IsZero() const {
        return std::any_of(begin(), end(), [](auto& m) { return m.IsZero(); });
    }

    size_t Product::FillPolynomialCoefficients(std::vector<Valuable>& coefficients, const Variable& v) const {
        size_t grade = 0;
        std::vector<Valuable> productCoefficients;
        std::vector<Valuable> memberCoefficients;
        for (auto& item : members) {
            auto g = item.FillPolynomialCoefficients(memberCoefficients, v);
            if (g > grade)
            {
                grade = g;
                if (productCoefficients.empty())
                    productCoefficients = std::move(memberCoefficients);
                else{
                    auto memberCoefficientsSize = memberCoefficients.size();
                    for (size_t i = 0; i < memberCoefficientsSize; ++i) {
                        if (i == productCoefficients.size()) {
                            productCoefficients.emplace_back(std::move(memberCoefficients[i]));
                        } else {
                            productCoefficients[i] *= std::move(memberCoefficients[i]);
                        }
                    }
                }
                memberCoefficients.clear();
            }
        }
        auto productCoefficientsSize = productCoefficients.size();
        if (coefficients.size() < productCoefficientsSize)
            coefficients.resize(productCoefficientsSize);
        for (size_t i = 0; i < productCoefficientsSize; ++i)
            coefficients[i] += std::move(productCoefficients[i]);
        return grade;
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
                    if (c != constants::one) {
                        _ *= c;
                    }
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
        // Special case for Sum comparison - Product never comes before Sum
        if (v.IsSum()) {
            return false;
        }
        
        if (size() == 1 && !v.IsProduct()) {
            return begin()->IsComesBefore(v);
        }

        auto mae = getMaxVaExp();
        auto vme = v.getMaxVaExp();
        
        if (mae != vme)
            return mae > vme;
        
        char vp[sizeof(Product)];
        auto isSameType = v.IsProduct();
        if(!isSameType)
        {
            return poc(*this, v);
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

            if (vaExpsSum != p->vaExpsSum)
            {
                return vaExpsSum > p->vaExpsSum;
            }
            
            auto collectionsAreSame = std::equal(begin(), end(), p->begin(), p->end(), std::equal_to<Valuable>());
            if (collectionsAreSame)
                return false;
            
            if (members.size() != p->members.size()) {
                return members.size() > p->members.size();
            }
            
            auto beg = begin();
            auto pbeg = p->begin();
            for (auto i1 = beg, i2 = pbeg; i1 != members.end(); ++i1, ++i2) {
                auto it1 = ProductOrderComparator::Order(*i1);
                auto it2 = ProductOrderComparator::Order(*i2);
                if (it1 != it2) {
                    return it1 < it2;
                }
            }

            for (auto i1 = beg, i2 = pbeg; i1 != end(); ++i1, ++i2) {
                if (!i1->Same(i2->get())) {
                    auto cmp12 = poc(*i1, *i2);
                    auto cmp21 = poc(*i2, *i1);
                    if (cmp12 == cmp21)
                        continue;
                    return cmp21;
                }
            }

            for (auto i1 = beg, i2 = pbeg; i1 != members.end(); ++i1, ++i2) {
                if (!i1->Same(i2->get())) {
                    return i1->IsComesBefore(*i2);
                }
            }

            // same types set, compare by value
            for (auto i1 = beg, i2 = pbeg; i1 != end(); ++i1, ++i2) {
                if (!i1->Same(i2->get())) {
                    return ValueOrderComparator(*i1, *i2);
                }
            }
            
            // everything is equal, should not be so
            LOG_AND_IMPLEMENT("Specify ordering: " << *this << " <=> " << v);
        }
        else
            IMPLEMENT
    }
    
    Valuable& Product::gcd(const Product& product)
    {
        VarHost::NonZeroLogOffScope off;
        auto it1 = begin();
        auto it2 = product.begin();
        auto end1 = end();
        auto end2 = product.end();
        auto gcd = InCommonWith(product);
        if (gcd == constants::one) {
            Become(base::GCD(product));
        } else {
            auto remainder1 = Optimized(View::Flat) / gcd;
            auto remainder2 = product / gcd;
            if (remainder1.IsProduct()) {
                remainder1.as<Product>().base::gcd(remainder2);
            } else {
                remainder1.gcd(remainder2);
            }
            if (gcd.IsProduct()) {
                gcd.as<Product>().Add(std::move(remainder1));
            } else {
                gcd *= remainder1;
            }
            Become(std::move(gcd));
        }
        return *this;
    }
    
    Valuable& Product::gcd(const Valuable& value) {
        VarHost::NonZeroLogOffScope off;
        if (value.IsProduct()) {
            gcd(value.as<Product>());
        }
        else {
            auto it = std::find(begin(), end(), value);
            if (it != end()) {
                Become(Extract(it));
            } else {
                Become(base::GCD(value));
            }
        }
        return *this;
    }

    Valuable Product::GCD(const Valuable& value) const {
        auto copy = ptrs::make_shared<Product>(*this);
        copy->gcd(value);
        return Valuable(copy);
    }

    Valuable& Product::operator +=(const Valuable& v)
    {
        if(v.IsZero())
            return *this;
        if (*this == v)
            return *this *= 2;
        if(*this == -v)
            return Become(0_v);
        if (IsMultival() == YesNoMaybe::Yes && v.IsMultival() == YesNoMaybe::Yes) {
            solutions_t s;
            for (auto& m : Distinct())
                for (auto& item : v.Distinct())
                    s.emplace(m + item);
            return Become(Valuable(std::move(s)));
        }
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

	std::pair<bool, Valuable> Product::IsMultiplicationSimplifiable(const Valuable& v) const {
        std::pair<bool, Valuable> is;
        Product p{};
        if (v.IsSum()) {
            for (auto& m : members) {
                if (!is.first) {
                    if (m.IsExponentiation()) {
                        auto& e = m.as<Exponentiation>();
                        if (e.getBase().IsSum()) {
                            is = m.IsMultiplicationSimplifiable(v);
                            if (is.first) {
                                p.Add(is.second);
                                continue;
                            }
                        }
                    }
                }
				p.Add(m);
            }
            if (is.first) {
                is.second = p;
            } else
				is = v.IsMultiplicationSimplifiable(*this);
        } else {
            for (auto& m : members) {
                if (!is.first) {
                    auto mIs = v.IsMultiplicationSimplifiable(m);
                    if (mIs.first) {
                        is.first = mIs.first;
                        p.Add(mIs.second);
						continue;
                    }
                }
                p.Add(m);
            }
            if (!is.first) {
                p.Add(v);
            }
            is.second = p;
        }
        if(is.first)
        {
            is.second.optimize();
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
            if (is.first) {
                auto complexityHasImproved = is.second.Complexity() < Complexity() + v.Complexity();
                is.first = complexityHasImproved
					|| is.second.IsSum() // checked in Sum::IsMultiplicationSimplifiable
                    || is.second.str().length() < str().length() + v.str().length()
                    || (is.second.IsProduct() && is.second.as<Product>().members.size() < (members.size() + (v.IsProduct() ? v.as<Product>().members.size() : 1)));
                if (!is.first) {
                    LOG_AND_IMPLEMENT("IsMultiplicationSimplifiable: "
                                      << str() << '[' << Complexity() << "] * " << v.str() << '[' << v.Complexity()
                                      << "] = " << is.second.str() << '[' << is.second.Complexity() << ']');
                }
            }
#endif
        }
        return is;
    }

   std::pair<bool, Valuable> Product::IsSummationSimplifiable(const Product& prod) const {
       std::pair<bool, Valuable> is;
       auto vars1 = getVaVal();
       auto vars2 = prod.getVaVal();
       auto commonVars = vars1.InCommonWith(vars2);
       if (vars1 != constants::one && vars2 != constants::one && commonVars == constants::one) {
           is.first = {};
       } else {
           auto common = InCommonWith(prod);
           if (common != constants::one) {
               auto thisNoCommon = *this / common;
               if (!operator==(thisNoCommon) // multivalue scenarios
                   && thisNoCommon.Complexity() <= Complexity())
               {
                   auto vNoCommon = prod / common;
                   if (vNoCommon != prod && vNoCommon.Complexity() <= prod.Complexity()) {
                       is = thisNoCommon.IsSummationSimplifiable(vNoCommon);
                       if (is.first) {
                           is.second *= common;
                       }
                   }
               }
           }
       }
       return is;
   }

   std::pair<bool,Valuable> Product::IsSummationSimplifiable(const Valuable& v) const
   {
       std::pair<bool,Valuable> is;
       is.first = v.IsZero();
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
       } else if (v.IsModulo()) {
       } else if (v.IsSum()
               || v.IsPrincipalSurd()
               || v.IsNaN()
                 )
       {
           is = v.IsSummationSimplifiable(*this);
       } else if (v.IsProduct()) {
           is = IsSummationSimplifiable(v.as<Product>());
       } else if (v.IsVa()
                  || v.IsFraction()
                  ) {
           //OptimizeOn o;
           auto common = InCommonWith(v);
           if (common != 1) {
               auto thisNoCommon = *this / common;
               if (!operator==(thisNoCommon)) { //multivalue cases (example ((-17)/16)*(4^((1/2))) / (1/2)*(4^((1/2))) = ((-17)/16)*(4^((1/2)))
                   auto vNoCommon = v / common;
                   if (vNoCommon != v) {
                       // std::cout << thisNoCommon << ".IsSummationSimplifiable(" << vNoCommon << ')' << std::endl;
                       is = thisNoCommon.IsSummationSimplifiable(vNoCommon);
                       if (is.first) {
                           is.second *= common;
                       //} else {
                       //    auto behindTheBrackets = thisNoCommon + vNoCommon;
                       //    is = common.IsMultiplicationSimplifiable(behindTheBrackets);
                       }
                   }
               }
           }
       } else if (v.Is_i()) {
           auto it = GetFirstOccurence<MinusOneSurd>();
           is.first = it != end();
           if (is.first) {
               auto remainder = *this / v;
               is = remainder.IsSummationSimplifiable(constants::one);
           }
       } else {
//           std::cout << v <<std::endl;
           LOG_AND_IMPLEMENT("Unknown case (need to implement) in Product::IsSummationSimplifiable for " << *this << " and " << v);
       }
       return is;
   }

    Valuable& Product::operator *=(const Valuable& v)
    {
        if ((size() == 1 && *begin() == constants::one) || size() == 0)
           return Become(Valuable(v));

        if (v.IsInt()){
            if (v.IsZero()) {
                return Become(0);
            } else if (v == constants::one) {
                return *this;
            } else {
                auto b = begin();
                if(b->IsSimple()){
                    Update(b, *b * v);
                    optimize();
                    return *this;
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
                    optimize();
                    return *this;
                }
                else if (it->IsExponentiation())
                {
                    auto& e = it->as<Exponentiation>();
                    if (e.getBase() == va) {
                        Update(it, va ^ (e.getExponentiation()+1));
                        optimize();
                        return *this;
                    }
                }
            }
        }
        else if (v.IsExponentiation())
        {
            auto& exponentiation = v.as<Exponentiation>();
            auto& vExpBase = exponentiation.getBase();
            for (auto it = members.begin(); it != members.end();)
            {
                if (it->IsExponentiation() &&
                    it->as<Exponentiation>().getBase() == vExpBase)
                {
                    Update(it, *it*v);
                    optimize();
                    return *this;
                }
                else if (vExpBase == *it)
                {
                    Update(it, vExpBase ^ (exponentiation.getExponentiation() + 1));
                    optimize();
                    return *this;
                }
                else
                    ++it;
            }
        }
        else if (v.IsProduct())
        {
            if(operator==(v))
                sq();
            else
                for(auto& m : v.as<Product>())
                    base::Add(m);
            optimize();
            return *this;
        }
        else
        {
            for (auto it = members.begin(); it != members.end();)
            {
                if (it->OfSameType(v)) {
                    Update(it, *it * v);
                    optimize();
                    return *this;
                }
                else
                    ++it;
            }
        }
        
        // add new member
        Add(v);
        
        optimize();
        return *this;
    }

    Valuable& Product::operator /=(const Valuable& v)
    {
        if (v.IsProduct()) {
            {
                OptimizeOff off;
                for (auto& i : v.as<Product>())
                    *this /= i;
            }
            optimize();
            return *this;
        }
        else if (v.IsSimple() ) {
            if (v == constants::one) {
                return *this;
            } else if (v.IsConstant()) {
                auto it = std::find(begin(), end(), v);
                if (it != end()) {
                    Delete(it);
                    optimize();
                    return *this;
                }
            }
            auto first = begin();
            if (first->IsSimple()) {
                Update(first, *first / v);
                optimize();
                return *this;
            }
            else {
                return operator *=(v.Reciprocal());
            }
        }
        else
        {
            if (v.IsMultival() != YesNoMaybe::Yes) {
                auto it = std::find(begin(), end(), v);
                if (it != end()) {
                    auto va = v.FindVa();
                    if (va) {
                        va->getVaHost()->LogNotZero(v);
                    }
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
                    } else if (v.IsMultival() == YesNoMaybe::Yes) {
                        solutions_t s;
                        for (auto& m : it->Distinct())
                            for (auto& item : v.Distinct())
                                s.emplace(m / item);
                        Update(it, Valuable(std::move(s)));
                        optimize();
                        return *this;
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

    Valuable Product::Sign() const {
        Product memberSignsProduct;
        for(auto& m: members){
            memberSignsProduct.Add(m.Sign());
        }
        OptimizeOn on;
        Valuable sign(std::move(memberSignsProduct));
        sign.optimize();
        return sign;
    }

    Valuable Product::ToBool() const {
        if (size() == 1) {
			return begin()->ToBool();
		} else if (is_optimized() && !FindVa()) {
            return IsZero() ? constants::one : constants::zero;
        } else {
            return base::ToBool();
        }
    }

    bool Product::operator<(const Product& v) const {
        auto gcd = GCD(v);
        if (gcd == constants::one) {
            LOG_AND_IMPLEMENT(*this << "   <   " << v);
        }
        return *this / gcd < v / gcd;
    }

    bool Product::operator<(const Valuable& v) const{
        if (operator==(v)) {
            return {};
        }
        auto beg = begin();
        if (members.size() == 1) {
            return beg->operator<(v);
        }
        // Bool is; // TODO: Implement Bool type which stores either bool or Valuable that calculates the bool when it is not yet deducible
        auto noVars = std::all_of(beg, end(), [&](auto& m){
            return m.FindVa() == nullptr;
        });
        auto isLess = noVars;
        if(noVars){
            auto sign = Sign();
            auto vSign = v.Sign();
            isLess = sign < vSign;
            if(!isLess){
                if (sign > vSign || operator==(v)) {
                } else { // same signs
                    if (v.IsProduct()) {
                        isLess = operator<(v.as<Product>());
                    } else if (Has(v)) {
                        isLess = *this / v < constants::one;
                    } else {
                        auto bigger = rt::find_if(members, 
                            [&](auto& item) {
                                return v.operator<(item);
                            });
                        auto found = bigger != members.end(); 
                        if (found) {
                            auto rest = *this / *bigger;
                            if (rest >= constants::one) {
                                isLess = {};
                            } else {
                                LOG_AND_IMPLEMENT(*this << "   <   " << v);
                            }
                        } else {
                            LOG_AND_IMPLEMENT(*this << "   <   " << v);
                        }
                    }
                }
            }
        } else {
            LOG_AND_IMPLEMENT(*this << "   <   " << v);
        }
        return isLess;
    }

    bool Product::operator ==(const Product& value) const
    {
        bool same = {};
        auto& c1 = GetConstCont();
        auto& c2 = value.GetConstCont();
        auto sz1 = c1.size();
        auto sz2 = c2.size();
        auto sameSizes = sz1 == sz2;
        if(sameSizes) {
            same = c1 == c2;
        } else if (sz1-sz2==1 || sz2-sz1==1) {
            auto it1 = c1.begin(), it2 = c2.begin();
            auto e1 = c1.end(), e2 = c2.end();
            for(same = true;
                same && it1 != e1 && it2 != e2;
                )
            {
                if(it1->Same(constants::one)){
                    ++it1;
                }
                if(it2->Same(constants::one)){
                    ++it2;
                }

                if(it1 == e1 || it2 == e2)
                    continue;

                same = same && it1->Same(*it2);

                ++it1, ++it2;
            }

            same = same && it1 == e1 && it2 == e2;
        } else {
            same = {};
        }

        // TODO: Check if it has same multival exponentiation and different sign or i in coefficient
        //if (!same 
        //    && IsMultival() == YesNoMaybe::Yes
        //    && value.IsMultival() == YesNoMaybe::Yes)
        //{
        //    LOG_AND_IMPLEMENT("Check if it has same multival exponentiation and different sign or i in coefficient: " << *this << " == " << v);
        //}

        return same;
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
        if (same) {
            same = operator ==(v.as<Product>());
        }
        else if (size_t sz1; sameHash
                             && ((sz1 = c1.size()) == 1
                                 || (sz1 == 2 && c1.begin()->Same(1_v) )))
        {
            same = c1.rbegin()->operator==(v);
        } else if (members.empty()) {
            same = v == 1;
        } else if (v.IsExponentiation()) {
            if (Has(v)) {
                auto sz1 = c1.size();
                same = sz1 == 1;
                if (!same) {
                    auto& e = v.as<Exponentiation>();
                    same = e.IsMultiSign() && sz1 == 2 && Has(constants::minus_1);
                    if (!same && Has(constants::i)) {
                        auto& ee = e.getExponentiation();
                        if (ee.IsFraction()) {
                            auto& eef = ee.as<Fraction>();
                            auto& eefdn = eef.getDenominator();
                            same = eefdn >= 4 || eefdn <= -4;
                        } else {
                            LOG_AND_IMPLEMENT("Examine new multisign form: " << *this << " == " << e);
                        }
                    }
                }
            }
            else {
                auto& e = v.as<Exponentiation>();
                auto& ee = e.getExponentiation();
                if (ee.IsSimple() && ee < constants::zero) {
                    OptimizeOn on;
                    auto potentiallyAlternativeForm = e.getBase().Reciprocal() ^ -ee;
                    if (!potentiallyAlternativeForm.Same(v)) {
                        same = operator==(potentiallyAlternativeForm);
                    }
                }
            }
        } else if (v.IsProduct()) {
            same = operator==(v.as<Product>());
            if (IsMultival() == YesNoMaybe::Yes && v.IsMultival() == YesNoMaybe::Yes)
            {
                // TODO: Check if it has same multival exponentiation and different sign or i in coefficient
                // LOG_AND_IMPLEMENT("Check if it has same multival exponentiation and different sign or i in
                // coefficient: " << *this << " == " << v);
            }
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
        return operator()(va, constants::zero);
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
                    s = ((*this / (it->first ^ it->second)) / augmentation) ^ (-it->second).Reciprocal();
                }
                else
                {
                    s = (augmentation / (*this / (it->first ^ it->second))) ^ it->second.Reciprocal();
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
                if (e->getExponentiation().IsZero()) {
                    IMPLEMENT
                }
                solutions.insert(0_v);
            }
            else {
                for (auto& member : members) {
                    member.solve(va, solutions);
                }
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

    std::ostream& Product::code(std::ostream& out) const
    {
        std::stringstream s;
        constexpr char sep[] = "*";
        for (auto& b : members)
            b.code(s) << sep;
        auto str = s.str();
        auto cstr = const_cast<char*>(str.c_str());
        cstr[str.size() - sizeof(sep) + 1] = 0;
        out << '(' << cstr << ')';
        return out;
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
    
    Valuable::vars_cont_t Product::GetVaExps() const {
        vars_cont_t vaExps;
        for (auto& m : members) {
            auto mVaExps = m.GetVaExps();
            for (auto& mve : mVaExps) {
                vaExps[mve.first] += mve.second;
            }            
        }
        return vaExps;
    }

    const PrincipalSurd* Product::PrincipalSurdFactor() const {
        const PrincipalSurd* surd = nullptr;
        GetFirstOccurence([&](auto& item) {
            surd = item.PrincipalSurdFactor();
            return surd != nullptr;
        });
        return surd;
    }
