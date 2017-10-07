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

    virtual std::ostream& print(std::ostream& out) const;
    
    Valuable() = default;
    
    virtual Valuable& Become(Valuable&& i);
    
public:
    explicit Valuable(Valuable* v);

    Valuable& operator =(const Valuable& v);
    Valuable& operator =(const Valuable&& v);
    Valuable(const Valuable& v);
    Valuable(Valuable&& v);
    Valuable(int i);

    virtual ~Valuable();
    virtual Valuable operator -() const;
    virtual Valuable& operator +=(const Valuable&);
    virtual Valuable& operator +=(int);
    virtual Valuable& operator *=(const Valuable&);
    virtual Valuable& operator /=(const Valuable&);
    virtual Valuable& operator %=(const Valuable&);
    virtual Valuable& operator--();
    virtual Valuable& operator++();
    //todo: operator^
    virtual bool operator<(const Valuable& number) const;
    virtual bool operator==(const Valuable& number) const;
    virtual Valuable abs() const;
    virtual void optimize(); /// if it simplifies than it should become the type
    virtual Valuable sqrt() const;
    virtual const Variable* FindVa() const;
    friend std::ostream& operator<<(std::ostream& out, const Valuable& obj);
    
    bool OfSameType(const Valuable& v) const;
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
    using Valuable::Valuable;
	ValuableDescendantContract() {}
    ValuableDescendantContract(ValuableDescendantContract&&){}
    ValuableDescendantContract(const ValuableDescendantContract&){}
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
        virtual const cont& GetCont() const = 0;
        
    public:
        
        auto begin() const
        {
            return GetCont().begin();
        }
        
        auto end() const
        {
            return GetCont().end();
        }
        
        auto begin()
        {
            return GetCont().begin();
        }
        
        auto end()
        {
            return GetCont().end();
        }
        
        size_t size() const
        {
            return GetCont().size();
        }
        
        virtual void Add(typename ContT::const_reference item) = 0;

        template<class T>
        const T* GetFirstOccurence() const
        {
            for(const auto& a : GetCont())
            {
                auto v = T::cast(a);
                if(v)
                    return v;
            }
            return nullptr;
        }
        
        bool HasValueType(const std::type_info& type) const
        {
            for(const auto& a : GetCont())
            {
                if(typeid(a) == type)
                    return true;
            }
            return false;
        }
        
        const Variable* FindVa() const
        {
            return GetFirstOccurence<Variable>();
        }
    };
}}

namespace std
{
	omnn::extrapolator::Valuable abs(const omnn::extrapolator::Valuable& v);
	omnn::extrapolator::Valuable sqrt(const omnn::extrapolator::Valuable& v);
}
