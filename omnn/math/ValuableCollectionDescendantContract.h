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
        ValuableCollectionDescendantContract(ValuableCollectionDescendantContract&&)=default;
        ValuableCollectionDescendantContract(const ValuableCollectionDescendantContract&)=default;

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
                if(typeid(a) == type)
                    return true;
            return false;
        }
        
        bool Has(const Valuable& v) const
        {
            auto& c = GetConstCont();
            // c.find uses order comparator
            auto found = std::find(c.begin(), c.end(), v); // using simple equality finder std::find instead
            auto foundSome = found != c.cend();
            auto has = foundSome && found->Same(v);
            if(foundSome && !has){
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
        void CollectVaNames(std::map<std::string, Variable>& s) const override {
            for (auto& i : GetConstCont())
                i.CollectVaNames(s);
        }
        
        Valuable Each(const std::function<Valuable(const Valuable&)>& m) const {
            Chld c;
            for(auto& i:GetConstCont())
                c.Add(m(i));
            return c;
        }

        Valuable::YesNoMaybe IsMultival() const override {
            if(size()==0) {
                IMPLEMENT
            }
            auto it = begin();
            auto is = it->IsMultival();
            if(size()!=1)
                while(++it != end())
                    is = is || it->IsMultival();
            return is;
        }
        
        bool eval(const std::map<Variable, Valuable>& with) override {
            bool evaluated = {};
            auto& members = GetCont();
            Valuable::SetView(Valuable::View::Calc);
            Chld c;
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

        /// For optimized values
        bool operator ==(const Valuable& v) const override
        {
            return Valuable::hash == v.Hash()
                && v.Is<Chld>()
                && GetConstCont() == v.as<Chld>().GetConstCont();
        }
    };
}}
