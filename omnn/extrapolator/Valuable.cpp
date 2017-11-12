//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Valuable.h"
#include "Accessor.h"
#include "Integer.h"

namespace omnn{
namespace extrapolator {

    thread_local bool Valuable::optimizations = true;
    
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

    /// gets valuable under accessor if accessor passed
    const Valuable* Valuable::accessor_cast(const Valuable& mayBeAccessor)
    {
        auto a = Accessor::cast(mayBeAccessor);
        //todo:CollecctionAccessor
        return a ? &a->v : nullptr;
    }
    
    template<>
    const Accessor* Valuable::cast(const Valuable& v)
    {
        auto e = v.exp;
        while (e && e->exp) e = e->exp;
        return dynamic_cast<const Accessor*>(e ? e.get() : &v);
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
        hash = std::move(v.hash);
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
            exp(std::move(v.exp)),
            hash(std::move(v.hash))
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
        if(exp) {
            Valuable& o = exp->operator+=(v);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
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
        if(exp) {
            Valuable& o = exp->operator%=(v);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else
            IMPLEMENT
    }

    Valuable& Valuable::operator--()
    {
        if(exp) {
            Valuable& o = exp->operator--();
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else
            IMPLEMENT
    }
    
    Valuable& Valuable::operator++()
    {
        if(exp) {
            Valuable& o = exp->operator++();
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else
            IMPLEMENT
    }
    
    Valuable& Valuable::operator^=(const Valuable& v)
    {
        if(exp) {
            Valuable& o = exp->operator^=(v);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
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
            if (optimizations)
            {
                exp->optimize();
                while (exp->exp) {
                    exp = exp->exp;
                }
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
    
    void Valuable::CollectVa(std::set<Variable>& s) const
    {
        if (exp)
            exp->CollectVa(s);
        else
            IMPLEMENT
    }

    void Valuable::Eval(const Variable& va, const Valuable& v)
    {
        if (exp) {
            exp->Eval(va, v);
            while (exp->exp) {
                exp = exp->exp;
            }
            return;
        }
        IMPLEMENT
    }
    
    bool Valuable::OfSameType(const Valuable& v) const
    {
        const Valuable& v1 = *cast<Valuable>(*this);
        const Valuable& v2 = *cast<Valuable>(v);
        return typeid(v1) == typeid(v2);
    }
    
    bool Valuable::Same(const Valuable& v) const
    {
        return OfSameType(v) && operator==(v);
    }
    
    size_t Valuable::Hash() const
    {
        return exp
            ? exp->Hash()
            : hash;
    }
    
    std::string Valuable::str() const
    {
        std::stringstream s;
        print(s);
        return s.str();
    }
    
    
    // store order operator
    bool HashCompare::operator()(const Valuable& v1, const Valuable& v2)
    {
        return v1.Hash() < v2.Hash();
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
