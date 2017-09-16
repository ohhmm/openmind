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
    virtual Valuable* Clone() const { return new Valuable(*this); }
    Valuable() = default;

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
    
    virtual ~Valuable();
    virtual self operator -(const self& v) const;
    virtual Valuable& operator +=(const Valuable& number);
    virtual Valuable& operator *=(const Valuable& number);
    virtual Valuable& operator /=(const Valuable& number);
    virtual Valuable& operator %=(const Valuable& number);
    virtual Valuable& operator--();
    virtual Valuable& operator++();
    virtual bool operator<(const Valuable& number) const;
    virtual bool operator==(const Valuable& number) const;

    friend std::ostream& operator<<(std::ostream& out, const Valuable& obj);
};

template <class Chld>
class ValuableDescendantContract : public Valuable
{
    friend Chld;
protected:
    Valuable* Clone() const override
    {
        return new Chld(*static_cast<const Chld*>(this));
    }

public:
    using Valuable::Valuable;
};

}}
