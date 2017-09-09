//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include "OpenOps.h"
#include <type_traits>
#include <boost/any.hpp>

namespace omnn{
namespace extrapolator {

class Valuable
        : public OpenOps<Valuable>
{
	using self = Valuable;
    
protected:
    boost::any exp;
    virtual boost::any getSelf() const;
    void setEncapsulatedInstance(const boost::any& e) { exp =e; }
    Valuable(boost::any&& any) : exp(any) {}
    Valuable() = default;

public:
    Valuable& operator=(const Valuable& v)
    {
        const auto& e = v.getEncapsulatedInstance();
        if(e.empty())
        {
            auto tv = typeid(v).name();
            auto ValuableTypeId = typeid(Valuable).name();
            if(tv != ValuableTypeId)
                exp = v.getSelf();
        }
        else
        {
            exp = e;
        }
    }
    Valuable(const Valuable& v)
    {
        *this = v;
    }
    Valuable(Valuable& v) {
        *this = v;
    }
    
    Valuable(Valuable&& v)
    : exp(std::move(v.getEncapsulatedInstance().empty() ? v.getSelf() : v.getEncapsulatedInstance()))
    { }
    
    template<class Descendant>
    Valuable(Descendant&& v) : exp(std::move(v))
    {
        static_assert(std::is_base_of<Valuable, Descendant>::value ||
                      std::is_same<Valuable, Descendant>::value,
                      "");
    }
    
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

    const boost::any& getEncapsulatedInstance() const { return exp; }

    friend std::ostream& operator<<(std::ostream& out, const Valuable& obj);
};

template <class Chld>
class ValuableDescendantContract : public Valuable
{
    friend Chld;
protected:
    boost::any getSelf() const override
    {
        return *(Chld*)this;
    }
    
    static const Chld* getEncapsulatedInstance(const Valuable& from)
    {
        return boost::any_cast<Chld>(&from.getEncapsulatedInstance());
    }
public:
    using Valuable::Valuable;
    
    ValuableDescendantContract& operator=(const ValuableDescendantContract& v)
    {
        setEncapsulatedInstance(((const Valuable&)v).getEncapsulatedInstance());
    }
    ValuableDescendantContract(const ValuableDescendantContract& v)
    {
        *this = v;
    }
    ValuableDescendantContract(ValuableDescendantContract& v) {
        *this = v;
    }
};
    
}}
