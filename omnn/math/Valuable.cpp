//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Valuable.h"
#include "Accessor.h"
#include "Fraction.h"
#include "Integer.h"
#include "Variable.h"
#include <boost/numeric/conversion/converter.hpp>


namespace omnn{
namespace math {

    thread_local bool Valuable::optimizations = true;
    
    void implement()
    {
        throw "Implement!";
        return;
    }
    
    Valuable* Valuable::Clone() const
    {
        if (exp)
            return exp->Clone();
        else
            IMPLEMENT
    }

    int Valuable::getTypeSize() const
    {
        assert(typeid(*this)==typeid(Valuable));
        return sizeof(Valuable);
    }
    
    Valuable::operator std::type_index() const
    {
        return exp ? static_cast<std::type_index>(*exp) : std::type_index(typeid(*this));
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
        new(this) Valuable(std::move(toMove)); // todo : not neccesarily wrap into Valuable if there is space. it need to be checked through through typeid
        return *this;
    }

    Valuable& Valuable::operator =(const Valuable&& v)
    {
        exp = std::move(v.exp);
        hash = std::move(v.hash);
        maxVaExp = std::move(v.maxVaExp);
        if (!exp)
            exp.reset(v.Clone());
        return *this;
    }

    Valuable& Valuable::operator =(const Valuable& v)
    {
        exp.reset(v.Clone());
        return *this;
    }

    Valuable::Valuable(Valuable&& v) :
            exp(std::move(v.exp)),
            hash(std::move(v.hash)),
            maxVaExp(std::move(v.maxVaExp))
    {
        if (!exp)
            exp.reset(v.Clone());
    }

    Valuable::Valuable(const Valuable& v) :
            exp(v.Clone())
    {
    }

    Valuable::Valuable(Valuable* v) :
            exp(v)
    {
    }

    Valuable::Valuable(double d) : exp(new Fraction(d)) {}
    Valuable::Valuable(int i) : exp(new Integer(i)) {}
    Valuable::Valuable(const long i) : exp(new Integer(i)) {}
    Valuable::Valuable(unsigned long i) : exp(new Integer(i)) {}
    Valuable::Valuable(unsigned long long i) : exp(new Integer(i)) {}

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
    
    Valuable& Valuable::d(const Variable& x)
    {
        if(exp) {
            Valuable& o = exp->d(x);
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

    bool Valuable::IsInt() const
    {
        return exp ? exp->IsInt() : false;
    }
    bool Valuable::IsFraction() const
    {
        return exp ? exp->IsFraction() : false;
    }
    bool Valuable::IsFormula() const
    {
        return exp ? exp->IsFormula() : false;
    }
    bool Valuable::IsExponentiation() const
    {
        return exp ? exp->IsExponentiation() : false;
    }
    bool Valuable::IsVa() const
    {
        return exp ? exp->IsVa() : false;
    }
    bool Valuable::IsProduct() const
    {
        return exp ? exp->IsProduct() : false;
    }
    bool Valuable::IsSum() const
    {
        return exp ? exp->IsSum() : false;
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
    
    Valuable Valuable::calcFreeMember() const
    {
        if(exp)
            return exp->calcFreeMember();
        else
            IMPLEMENT
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
    
    bool Valuable::HasVa(const Variable& x) const
    {
        if (exp) {
            return exp->HasVa(x);
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
 
    bool Valuable::HasSameVars(const Valuable& v) const
    {
        std::set<Variable> thisVa, vVa;
        this->CollectVa(thisVa);
        v.CollectVa(vVa);
        return thisVa == vVa;
    }
    
    bool Valuable::IsMonicPolynomial() const
    {
        std::set<Variable> vars;
        CollectVa(vars);
        return vars.size() == 1;
    }
    
    int Valuable::getMaxVaExp() const
    {
        return exp ? exp->getMaxVaExp() : maxVaExp;
    }
    
    bool Valuable::IsComesBefore(const Valuable& v) const
    {
        if (exp)
            return exp->IsComesBefore(v);
        else
            IMPLEMENT
    }
    
    const Valuable::vars_cont_t& Valuable::getCommonVars() const
    {
        if (exp)
            return exp->getCommonVars();
        else
            IMPLEMENT
    }
    
    Valuable Valuable::varless() const
    {
        return *this / getVaVal();
    }
    
    Valuable Valuable::VaVal(const vars_cont_t& v)
    {
        Valuable p(1);
        for(auto& kv : v)
        {
            p *= kv.first ^ kv.second;
        }
        p.optimize();
        return p;
    }
    
    Valuable Valuable::getVaVal() const
    {
        return VaVal(getCommonVars());
    }
    
    const Valuable::vars_cont_t& Valuable::emptyCommonVars()
    {
        static const Valuable::vars_cont_t _;
        return _;
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
    
    Valuable::operator size_t() const
    {
        if (exp)
            return exp->operator size_t();
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

    Valuable::operator unsigned char() const
    {
        if (exp)
            return exp->operator unsigned char();
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
}}

namespace std
{
    ::omnn::math::Valuable abs(const ::omnn::math::Valuable& v)
    {
        return v.abs();
    }
    ::omnn::math::Valuable sqrt(const ::omnn::math::Valuable& v)
    {
        return v.sqrt();
    }
}

::omnn::math::Valuable operator"" _v(unsigned long long v)
{
    return ::omnn::math::Integer(boost::multiprecision::cpp_int(v));
}

::omnn::math::Valuable operator"" _v(long double v)
{
    return ::omnn::math::Fraction(v);
}
