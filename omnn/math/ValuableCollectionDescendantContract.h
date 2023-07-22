//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include "Exponentiation.h"
#include <algorithm>
#include <future>
#include <iterator>

namespace omnn{
namespace math {

    template <class ChildT, class ContT>
    class ValuableCollectionDescendantContract : public ValuableDescendantContract<ChildT>
    {
        using base = ValuableDescendantContract<ChildT>;
        
    protected:
        using cont = ContT;
        virtual cont& GetCont() = 0;
        bool IsSubObject(const Valuable& o) const override {
            auto is = this == &o.get();
            if (!is) {
                for(auto& m : GetConstCont()) {
                    is = &m.get() == &o.get();
                    if(is)
                        break;
                }
            }
            return is;
        }
        
    public:
        using iterator = typename cont::iterator;
        using const_reference = typename ContT::const_reference;

        using base::base;
        ValuableCollectionDescendantContract(ValuableCollectionDescendantContract&&)=default;
        ValuableCollectionDescendantContract(const ValuableCollectionDescendantContract&)=default;
        ValuableCollectionDescendantContract& operator=(ValuableCollectionDescendantContract&&)=default;
        ValuableCollectionDescendantContract& operator=(const ValuableCollectionDescendantContract&)=default;

        virtual const cont& GetConstCont() const = 0;
        
        auto begin() { return GetCont().begin(); }
        auto end() { return GetCont().end(); }
        auto begin() const { return GetConstCont().begin(); }
        auto end() const { return GetConstCont().end(); }
		auto cbegin() const { return GetConstCont().cbegin(); }
        auto cend() const { return GetConstCont().cend(); }
        auto rbegin() { return GetCont().rbegin(); }
        auto rend() { return GetCont().rend(); }
        auto crbegin() const { return GetConstCont().crbegin(); }
        auto crend() const { return GetConstCont().crend(); }

        size_t size() const
        {
            return GetConstCont().size();
        }
        
        static iterator getit(iterator it){
            return it;
        }
        iterator getit(const std::pair<iterator,bool>& it){
            auto inserted = it.second;
            if(!inserted){
                return Had(it.first);
            }
            return it.first;
        }
        virtual iterator Had(iterator it)
        {
            IMPLEMENT
        }
        
        virtual const iterator Add(const Valuable& item, const iterator hint)
        {
            Valuable::hash ^= item.Hash();
            auto& c = GetCont();
            this->optimized = {};
            auto it = hint == c.end() ? getit(c.emplace(item)) : getit(c.insert(hint, item));
            return it;
        }

        template <class ItemT>
        const iterator Add(ItemT&& item)
        {
            return Add(std::forward<ItemT>(item), GetCont().end());
        }

        template<class T>
        auto GetFirstOccurence() const
        {
            auto& c = GetConstCont();
            auto i = c.begin();
            for(auto e = c.end(); i != e; ++i)
                if(i->template Is<T>())
                    break;
            return i;
        }
        
        a_int Complexity() const override {
            a_int c = 0;
            for(auto& m : GetConstCont())
                c += m.Complexity();
            return c;
        }
        
        bool HasValueType(const std::type_info& type) const
        {
            for(const auto& a : GetConstCont())
                if(typeid(a.get()) == type)
                    return true;
            return false;
        }
        
        template<class T>
        bool HasValueType() const
        {
            auto& cc = GetConstCont();
            return std::any_of(
                cc.begin(), cc.end(),
                [](auto& m) {
                    return m.Is<T>();
                });
        }
        
        bool Has(const Valuable& v) const
        {
            auto& c = GetConstCont();
            // c.find uses order comparator
            auto found = std::find(c.begin(), c.end(), v); // using simple equality finder std::find instead
            auto foundSome = found != c.cend();
            auto has = foundSome && found->Same(v);
            if(foundSome && !has){
#ifndef NDEBUG
                //found->Same(v);
#endif
                LOG_AND_IMPLEMENT("Bug in comparator: " << this->str() << " has " << v);
            }
            return has;
        }
        
        const Variable* FindVa() const override
        {
            for (auto& i : GetConstCont())
            {
                auto va = i.FindVa();
                if (va)
                    return va;
            }
            return nullptr;
        }
        
        bool HasVa(const Variable& va) const override
        {
            for (auto& i : GetConstCont())
                if(i.HasVa(va))
                    return true;
            return false;
        }
        
        void CollectVa(std::set<Variable>& s) const override {
            for (auto& i : GetConstCont())
                i.CollectVa(s);
        }
        void CollectVaNames(Valuable::va_names_t& s) const override {
            for (auto& i : GetConstCont())
                i.CollectVaNames(s);
        }
        
        Valuable Each(const std::function<Valuable(const Valuable&)>& m) const {
            ChildT c;
            for(auto& i:GetConstCont())
                c.Add(m(i));
            return c;
        }

        bool IsSimple() const override {
            return std::all_of(begin(), end(), [](auto& m){return m.IsSimple();});
        }

        Valuable::YesNoMaybe IsMultival() const override {
            Valuable::YesNoMaybe is;
            if(size()==0) {
                is = Valuable::YesNoMaybe::No;
            } else {
            auto it = begin();
                is = it->IsMultival();
            if(size()!=1)
                while(++it != end())
                    is = is || it->IsMultival();
            }
            return is;
        }
        
        void Values(const std::function<bool(const Valuable&)>& fun) const override {
            auto& c = GetConstCont();
            auto count = c.size();
            auto nTotal = count;
            auto sharedValuesProjection = std::vector<std::vector<Valuable>>();
            for (auto& m : c) {
                if (m.IsMultival() == Valuable::YesNoMaybe::Yes) {
                    if (sharedValuesProjection.size()) {
                        auto sharedValuesProjectionCopy = std::move(sharedValuesProjection);
                        m.Values([&](const Valuable& value) {
                            decltype(sharedValuesProjection) copy = sharedValuesProjectionCopy;
                            for (auto& copyMember : copy) {
                                copyMember.emplace_back(value);
                            }
                            std::move(copy.begin(), copy.end(), std::back_inserter(sharedValuesProjection));
                            return true;
                        });
                    } else {
                        m.Values([&](const Valuable& value) { 
                            sharedValuesProjection.emplace_back().emplace_back(value.Link());
                            return true;
                        });
                    }
                } else {
                    if (sharedValuesProjection.size()) {
                        for (auto& projection : sharedValuesProjection) {
                            projection.emplace_back(m.Link());
                        }
                    } else {
                        sharedValuesProjection.emplace_back().emplace_back(m.Link());
                    }
                }
            }

            for (auto&& projection : sharedValuesProjection) {
                ChildT value;
                for (auto& item : projection) {
                    value.Add(item);
                }
                fun(value);
            }
        }

        bool eval(const std::map<Variable, Valuable>& with) override {
            bool evaluated = {};
            auto& members = GetCont();
            Valuable::SetView(Valuable::View::Calc);
            ChildT c;
            if (members.size() < 100) {
                for(auto&& cm:GetCont()){
                    auto&& m = const_cast<Valuable&&>(cm);
                    evaluated = m.eval(with) || evaluated;
                    c.Add(std::move(m));
                }
            } else {
                std::deque<std::future<bool>> jobs;
                for(auto& m : members) {
                    jobs.push_back(std::async(std::launch::async, [&]() {
                        return const_cast<Valuable&>(m).eval(with);
                    }));
                }
                for(auto&& m : members) {
                    auto& f = jobs.front();
                    evaluated = f.get() || evaluated;
                    c.Add(std::move(const_cast<Valuable&&>(m)));
                    jobs.pop_front();
                }
            }
            Valuable::Become(std::move(c));
            return evaluated;
        }
        
        void Eval(const Variable& va, const Valuable& v) override
        {
            Valuable::SetView(Valuable::View::Calc);
            auto& c = GetCont();
            auto e = c.end();
            bool updated;
            do
            {
                updated = {};
                for(auto i = c.begin(); i != e;)
                {
                    if(i->HasVa(va)){
                        auto co = *i;
                        co.Eval(va,v);
                        co.optimize();
                        Update(i,co);
                        updated = true;
                    } else {
                        ++i;
                    }
                }
            } while (updated);
            
//            if(!FindVa())
//                this->optimize();
        }

        virtual void Update(iterator& it, const Valuable& v)
        {
            Delete(it);
            it=Add(v, it);
            Valuable::optimized = {};
        }

        virtual void Delete(iterator& it)
        {
            Valuable::hash ^= it->Hash();
            auto& c = GetCont();
            c.erase(it++);
            Valuable::optimized = {};
        }
    };
}}
