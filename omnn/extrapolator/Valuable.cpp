//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Valuable.h"
#include "Accessor.h"
#include "Fraction.h"
#include "Integer.h"
#include <boost/numeric/conversion/converter.hpp>


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

    int Valuable::getTypeSize() const
    {
        assert(typeid(*this)==typeid(Valuable));
        return sizeof(Valuable);
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
        v.optimize();
        auto e = v.exp;
        if(e)
            while (e->exp) {
                e = e->exp;
            }
        Valuable& toMove = e ? *e.get() : v;
        auto sizeWas = getTypeSize();
        auto newSize = toMove.getTypeSize();

        this->~Valuable();        // before this call
        if (newSize <= sizeWas) {
            
        }
        new (this) Valuable(std::move(toMove)); // todo : not neccesarily wrap into Valuable if there is space. it need to be checked through through typeid
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
            return Hash()==v.Hash() && exp->operator==(v);
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
        return Hash()==v.Hash() && OfSameType(v) && operator==(v);
    }
    
    Valuable::operator bool() const
    {
        return *this != 0_v;
    }
    
    Valuable::operator int() const
    {
        if (exp)
            return exp->operator int();
        else
            IMPLEMENT
    }
    
    Valuable::operator double() const
    {
        if (exp)
            return exp->operator double();
        else
            IMPLEMENT
    }

    Valuable::operator long double() const
    {
        if (exp)
            return exp->operator double();
        else
            IMPLEMENT
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
    
    size_t hash_value(const Valuable& v)
    {
        return v.Hash();
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
    return ::omnn::extrapolator::Integer(boost::multiprecision::cpp_int(v));
}

::omnn::extrapolator::Valuable operator"" _v(long double v)
{
    return ::omnn::extrapolator::Fraction(v);
}
