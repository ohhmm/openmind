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
    
class Valuable
        : public OpenOps<Valuable>
    
{
	using self = Valuable;
    using encapsulated_instance = ptrs::shared_ptr<Valuable>;
    encapsulated_instance exp = nullptr;

protected:
    virtual Valuable* Clone() const {
        if(exp)
            return exp->Clone();
        else
            return new Valuable(*this);
    }

    template<class T>
    static const T* cast(const Valuable& v)
    {
        return v.exp? dynamic_cast<const T*>(v.exp.get())
                    : dynamic_cast<const T*>(&v);
    }

    virtual std::ostream& print(std::ostream& out) const;
    
    Valuable() = default;
    
    virtual Valuable& Become(Valuable&& i)
    {
        *this = Valuable(i.Clone());
        return *this;
    }
    
public:
    Valuable(Valuable* v) : exp(v) { }
//    Valuable* operator->() const
//    {
//        return exp ? exp.get() : const_cast<Valuable*>(this);
//    }

    Valuable& operator=(const Valuable& v)
    {
        exp.reset(v.exp ? v.exp.get() : v.Clone());
        return *this;
    }
    Valuable(const Valuable& v)
    : exp((v.exp ? v.exp.get() : const_cast<Valuable*>(&v))->Clone())
    {
    }
    Valuable(Valuable&& v)
        : Valuable(v.Clone())
    {}
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
    virtual void optimize();
    virtual Valuable sqrt() const;
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


};

    template <class Chld>
    class ValuableCollectionDescendantContract
        : public ValuableDescendantContract<Chld>
    {
        using base = ValuableDescendantContract<Chld>;
    public:
        // begin
        // end
        virtual size_t size() const = 0;
        bool HasValueType(const std::type_info& type) const
        {
            auto ch = base::cast(*this);
            for(const auto& a : *ch)
            {
                if(typeid(a) == type)
                    return true;
            }
            return false;
        }
    };
    
}}

namespace std
{
	omnn::extrapolator::Valuable abs(const omnn::extrapolator::Valuable& v);
	omnn::extrapolator::Valuable sqrt(const omnn::extrapolator::Valuable& v);
}
