//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include "OpenOps.h"
#include <memory>
#include <type_traits>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace omnn{
namespace extrapolator {
    
    namespace ptrs = ::std;

    class Integer;
    class Variable;
    struct ValuableDescendantMarker {};
    
class Valuable
        : public OpenOps<Valuable>
    
{
#ifdef BOOST_TEST_MODULE
public:
#endif
	using self = Valuable;
    using encapsulated_instance = ptrs::shared_ptr<Valuable>;
    encapsulated_instance exp = nullptr;

protected:
    virtual Valuable* Clone() const;

    template<class T>
    static const T* cast(const Valuable& v)
    {
        auto e = v.exp;
        while (e && e->exp) e = e->exp;
        return dynamic_cast<const T*>(e ? e.get() : &v);
    }
    
    template<class T> Valuable() {}
    Valuable(ValuableDescendantMarker){}
    
    virtual std::ostream& print(std::ostream& out) const;
    virtual Valuable& Become(Valuable&& i);
    
public:
    explicit Valuable(Valuable* v);
    
    Valuable& operator =(const Valuable& v);
    Valuable& operator =(const Valuable&& v);
    Valuable(const Valuable& v);
    Valuable(Valuable&& v);
    Valuable(int i = 0);

    virtual ~Valuable();
    virtual Valuable operator -() const;
    virtual Valuable& operator +=(const Valuable&);
    virtual Valuable& operator +=(int);
    virtual Valuable& operator *=(const Valuable&);
    virtual Valuable& operator /=(const Valuable&);
    virtual Valuable& operator %=(const Valuable&);
    virtual Valuable& operator--();
    virtual Valuable& operator++();
    virtual Valuable& operator^=(const Valuable&);
    virtual bool operator<(const Valuable& number) const;
    virtual bool operator==(const Valuable& number) const;
    virtual Valuable abs() const;
    virtual void optimize(); /// if it simplifies than it should become the type
    virtual Valuable sqrt() const;
    virtual const Variable* FindVa() const;
    virtual void Eval(const Variable& va, const Valuable& v);
    friend std::ostream& operator<<(std::ostream& out, const Valuable& obj);
    
    bool OfSameType(const Valuable& v) const;
    virtual size_t Hash() const;
};

template <class Chld>
class ValuableDescendantContract
        : public Valuable
{
    using self = ValuableDescendantContract;
    friend Chld;
    friend self;
protected:
    Valuable* Clone() const override
    {
        return new Chld(*static_cast<const Chld*>(this));
    }

public:
    // once compiler allow
    // todo :
    //ValuableDescendantContract() : Valuable<>() {}
    // instead of
    ValuableDescendantContract() : Valuable(ValuableDescendantMarker()) {}
    ValuableDescendantContract(ValuableDescendantContract&&) : Valuable(ValuableDescendantMarker()) {}
    ValuableDescendantContract(const ValuableDescendantContract&) : Valuable(ValuableDescendantMarker()) {}
	ValuableDescendantContract& operator=(const ValuableDescendantContract& f) { return *this; }
	ValuableDescendantContract& operator=(ValuableDescendantContract&& f) { return *this; }
    static const Chld* cast(const Valuable& v){
        return Valuable::cast<Chld>(v);
    }
    //friend Chld operator+(const Chld& c, int i) { return c+Chld(i); }
 
    
    Valuable abs() const override
    {
        auto i = const_cast<Chld*>(cast(*this));
        if(*i < Chld(0))
        {
            return *cast(-*i);
        }
		return *this;
    }
    void optimize() override { }
	Valuable sqrt() const override { throw "Implement!"; }

    const Variable* FindVa() const override { return nullptr; }
};

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
        
        void Eval(const Variable& va, const Valuable& v) override
        {
            for(auto& i : GetConstCont())
            {
                const_cast<Valuable&>(i).Eval(va,v);
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
    };
}}

namespace std
{
	omnn::extrapolator::Valuable abs(const omnn::extrapolator::Valuable& v);
	omnn::extrapolator::Valuable sqrt(const omnn::extrapolator::Valuable& v);
    
    template<>
    struct hash<omnn::extrapolator::Valuable> {
        size_t operator()(const omnn::extrapolator::Valuable& v) const {
            return v.Hash();
        }
    };
}

::omnn::extrapolator::Valuable operator"" _v(unsigned long long v);
