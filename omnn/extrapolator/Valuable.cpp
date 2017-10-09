//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Valuable.h"
#include "Integer.h"

namespace omnn{
namespace extrapolator {

    static void implement()
    {
        throw "Implement!";
    }
    
    #define IMPLEMENT { implement(); throw; }

    Valuable* Valuable::Clone() const
    {
        if (exp)
            return exp->Clone();
        else
            return new Valuable(*this);
    }

    Valuable& Valuable::Become(Valuable&& i)
    {
        Valuable v(std::move(i)); // move here in case it moved from the object member
        this->~Valuable();        // before this call
        new (this) Valuable(std::move(v)); // todo : not neccesarily wrap into Valuable if there is space. it need to be checked through typeid
        optimize();
        return *this;
    }

    Valuable& Valuable::operator =(const Valuable&& v)
    {
        exp = std::move(v.exp);
        if (!exp)
            exp.reset(v.Clone());
        return *this;
    }

    Valuable& Valuable::operator =(const Valuable& v)
    {
        exp.reset((v.exp ? v.exp.get() : const_cast<Valuable*>(&v))->Clone());
        return *this;
    }

    Valuable::Valuable(Valuable&& v) :
            exp(std::move(v.exp))
    {
        if (!exp)
            exp.reset(v.Clone());
    }

    Valuable::Valuable(const Valuable& v) :
            exp((v.exp ? v.exp.get() : const_cast<Valuable*>(&v))->Clone())
    {
    }

    Valuable::Valuable(Valuable* v) :
            exp(v)
    {
    }

    Valuable::Valuable(int i) : exp(new Integer(i)) {}

    Valuable::~Valuable()
    {
    }

    Valuable Valuable::operator -() const
    {
        if(exp)
            return exp->operator-();
        else
            IMPLEMENT
    }

    Valuable& Valuable::operator +=(const Valuable& v)
    {
        if(exp) {
            Valuable& o = exp->operator+=(v);
            if (o.exp) {
                exp = o.exp;
                return *this;
            }
            return o;
        }
        else
            IMPLEMENT
    }
    
    Valuable& Valuable::operator +=(int v)
    {
        if(exp)
            return exp->operator+=(v);
        else
            IMPLEMENT
        }

    Valuable& Valuable::operator *=(const Valuable& v)
    {
        if(exp) {
            Valuable& o = exp->operator*=(v);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else
            IMPLEMENT
    }

    Valuable& Valuable::operator /=(const Valuable& v)
    {
        if(exp) {
            Valuable& o = exp->operator/=(v);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
            IMPLEMENT
    }

    Valuable& Valuable::operator %=(const Valuable& v)
    {
        if(exp)
            return exp->operator%=(v);
        else
            IMPLEMENT
    }

    Valuable& Valuable::operator--()
    {
        if(exp)
            return exp->operator--();
        else
            IMPLEMENT
    }
    
    Valuable& Valuable::operator++()
    {
        if(exp)
            return exp->operator++();
        else
            IMPLEMENT
    }
    
    Valuable& Valuable::operator^=(const Valuable& v)
    {
        if(exp)
            return exp->operator^=(v);
        else
            IMPLEMENT
    }
    
    bool Valuable::operator<(const Valuable& v) const
    {
        if(exp)
            return exp->operator<(v);
        else
            IMPLEMENT
    }

    bool Valuable::operator==(const Valuable& v) const
    {
        if(exp)
            return exp->operator==(v);
        else
            IMPLEMENT
    }
    
    std::ostream& Valuable::print(std::ostream& out) const
    {
        if(exp)
            return exp->print(out);
        else
            IMPLEMENT
    }
    
    std::ostream& operator<<(std::ostream& out, const Valuable& obj)
    {
        return obj.print(out);
    }

    Valuable Valuable::abs() const
    {
        if(exp)
            return exp->abs();
        else
            IMPLEMENT
    }

    void Valuable::optimize()
    {
        if(exp) {
            while (exp->exp) {
                exp = exp->exp;
            }
            exp->optimize();
            while (exp->exp) {
                exp = exp->exp;
            }
            return;
        }
        else
            IMPLEMENT
    }

	Valuable Valuable::sqrt() const {
        if(exp)
            return exp->sqrt();
        else
            IMPLEMENT
    }
    
    const Variable* Valuable::FindVa() const
    {
        if (exp) {
            return exp->FindVa();
        }
        IMPLEMENT
    }

    bool Valuable::OfSameType(const Valuable& v) const
    {
        const Valuable& v1 = exp ? *exp : *this;
        const Valuable& v2 = v.exp ? *v.exp : v;
        return typeid(v1) == typeid(v2);
    }
}}

namespace std
{
    ::omnn::extrapolator::Valuable abs(const ::omnn::extrapolator::Valuable& v)
    {
        return v.abs();
    }
    ::omnn::extrapolator::Valuable sqrt(const ::omnn::extrapolator::Valuable& v)
    {
        return v.sqrt();
    }
}

::omnn::extrapolator::Valuable operator"" _v(unsigned long long v)
{
    return ::omnn::extrapolator::Integer(v);
}
