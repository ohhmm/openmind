//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include "Exponentiation.h"

namespace omnn{
namespace extrapolator {

    template <class Chld, class ContT>
    class ValuableCollectionDescendantContract
        : public ValuableDescendantContract<Chld>
    {
        using base = ValuableDescendantContract<Chld>;
        
    protected:
        using cont = ContT;
        virtual cont& GetCont() = 0;
        virtual const cont& GetConstCont() const = 0;
        
    public:

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
        
        size_t size() const
        {
            return GetConstCont().size();
        }
        
        virtual void Add(typename ContT::const_reference item) = 0;

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
            for(auto i = c.begin(); i != e;)
            {
                auto co = *i;
                co.Eval(va,v);
                if (!i->Same(co))
                {
                    c.erase(i++);
                    c.insert(i,co);
                }
                else
                {
                    ++i;
                }
            }
            this->optimize();
        }
        
        size_t Hash() const override
        {
            size_t h = 0;
            for (auto& i : GetConstCont())
            {
                h^=i.Hash();
            }
            return h;
        }
        
        Valuable& operator^=(const Valuable& v) override
        {
            return base::Become(Exponentiation(*this,v));
        }
    };
}}
