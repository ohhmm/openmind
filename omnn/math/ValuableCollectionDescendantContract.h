//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include "Exponentiation.h"

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
        
        virtual const cont& GetConstCont() const = 0;

//        template<class... T>
//        ValuableCollectionDescendantContract(const T&... vals)
//        {
//            for (const Valuable& arg : { Valuable(vals)... })
//            {
//                Add(arg);
//            }
//        }
        
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
        static iterator getit(std::pair<iterator,bool> it){
            auto inserted = it.second;
            if(!inserted)
                IMPLEMENT
            return it.first;
        }
        
        virtual const iterator Add(const_reference item)
        {
            Valuable::hash ^= item.Hash();
            auto it = GetCont().insert(item);
            return getit(it);
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
        
        void CollectVa(std::set<Variable>& s) const override
        {
            for (auto& i : GetConstCont())
                i.CollectVa(s);
        }
        
        void Eval(const Variable& va, const Valuable& v) override
        {
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
            auto& c = GetCont();
            Delete(it);
            it = c.insert(it, v);
            Valuable::hash ^= v.Hash();
        }

        virtual void Delete(iterator& it)
        {
            Valuable::hash ^= it->Hash();
            auto& c = GetCont();
            c.erase(it++);
        }

        Valuable& operator^=(const Valuable& v) override
        {
            return base::Become(Exponentiation(*this,v));
        }
        
        bool operator ==(const Valuable& v) const override
        {
            auto c = Chld::cast(v);
            return c
                && Valuable::hash == v.Hash()
                && GetConstCont()==c->GetConstCont();
        }
        
        template<class T>
        bool Visit(const std::function<void(Valuable&)>& f)
        {
            bool wasVisits = {};
            auto& c = GetCont();
            auto e = c.end();
            for(auto i = c.begin(); i != e; ++i)
            {
                auto v = T::cast(*i);
                if(v)
                {
                    f(CollectionAccessor(*v, *this));
                    wasVisits = true;
                }
            }
            return wasVisits;
        }
    };
}}
