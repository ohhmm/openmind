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
    Valuable() = default;
    
    template<class T>
    static const T* cast(const Valuable& v)
    {
        return v.exp? dynamic_cast<const T*>(v.exp.get())
                    : dynamic_cast<const T*>(&v);
    }

    virtual std::ostream& print(std::ostream& out) const;
    
public:
    Valuable(Valuable* v) : exp(v) { }
    Valuable* operator->() const
    {
        return exp ? exp.get() : const_cast<Valuable*>(this);
    }
    
    Valuable(const Valuable& v)
    : exp(v.exp ? v.exp.get() : v.Clone())
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
    virtual bool operator<(const Valuable& number) const;
    virtual bool operator==(const Valuable& number) const;
    virtual void optimize();
    friend std::ostream& operator<<(std::ostream& out, const Valuable& obj);
};

template <class Chld>
class ValuableDescendantContract
        : public Valuable
{
    using self = ValuableDescendantContract;
    friend Chld;
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
};

}}
