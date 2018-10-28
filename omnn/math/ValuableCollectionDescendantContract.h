//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include "Exponentiation.h"
#include <future>

namespace omnn{
namespace math {

    template <class Chld, class ContT>
    class ValuableCollectionDescendantContract
        : public ValuableDescendantContract<Chld>
    {
        using base = ValuableDescendantContract<Chld>;
        
    protected:
        using cont = ContT;
        virtual cont& GetCont() = 0;
        
    public:
        using iterator = typename cont::iterator;
        using const_reference = typename ContT::const_reference;

        using base::base;

        virtual const cont& GetConstCont() const = 0;
        
        auto begin()
        {
            return GetCont().begin();
        }

        auto end()
        {
            return GetCont().end();
        }

        auto begin() const
        {
            return GetConstCont().begin();
        }

        auto end() const
        {
            return GetConstCont().end();
        }

        auto rbegin() const
        {
            return GetConstCont().rbegin();
        }

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
            auto it = hint == c.end() ? getit(c.insert(item)) : getit(c.insert(hint, item));
            return it;
        }

        const iterator Add(const Valuable& item)
        {
            return Add(item, GetCont().end());
        }
        
        const iterator Add(Valuable&& item)
        {
            return Add(std::move(item), GetCont().end());
        }

        template<class T>
        auto GetFirstOccurence() const
        {
            auto& c = GetConstCont();
            auto e = c.end();
            for(auto i = c.begin(); i != e; ++i)
            {
                auto v = T::cast(*i);
                if(v)
                    return i;
            }
            return e;
        }
        
        bool HasValueType(const std::type_info& type) const
        {
            for(const auto& a : GetConstCont())
            {
                if(typeid(a) == type)
                    return true;
            }
            return false;
        }
        
        bool Has(const Valuable& v) const
        {
            for(const auto& a : GetConstCont())
            {
                if(a==v) return true;
            }
            return false;
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
        
        void CollectVa(std::set<Variable>& s) const override
        {
            for (auto& i : GetConstCont())
                i.CollectVa(s);
        }
        
        Valuable& eval(const std::map<Variable, Valuable>& with) override {
            auto& members = GetCont();
            Valuable::SetView(Valuable::View::Calc);
            Chld c;
            if (true || members.size() < 10) {
                for(auto&& m:GetCont()){
                    c.Add(std::move(const_cast<Valuable&&>(m).eval(with)));
                }
            } else {
                std::deque<std::future<Valuable>> jobs;
                for(auto& m : members) {
                    jobs.push_back(std::async([&with,
    //                                           &&m
                                               item=m
                                               ]() {
                        Valuable t = const_cast<Valuable&&>(std::move(item));
                        return std::move(t.eval(with));
                    }));
                }
                for (auto sz = jobs.size(); sz; ) {
                    --sz;
                    auto& f = jobs.front();
                    f.wait();
                    c.Add(f.get());
                    jobs.pop_front();
                }
            }
            Valuable::Become(std::move(c));
            return *this;
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
                    auto co = *i;
                    co.Eval(va,v);
                    if (!i->Same(co))
                    {
                        co.optimize();
                        Update(i,co);
                        updated = true;
                    }
                    else
                    {
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
        }

        virtual void Delete(iterator& it)
        {
            Valuable::hash ^= it->Hash();
            auto& c = GetCont();
            c.erase(it++);
        }

        bool operator ==(const Valuable& v) const override
        {
            return Valuable::hash == v.Hash()
                && v.Is<Chld>()
                && GetConstCont()==Chld::cast(v)->GetConstCont();
        }
    };
}}
