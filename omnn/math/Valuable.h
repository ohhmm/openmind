//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include "OpenOps.h"
#include <map>
#include <memory>
#include <set>
#include <typeindex>
#include <boost/shared_ptr.hpp>

namespace omnn{
namespace math {
    
    namespace ptrs = ::std;

    class Accessor;
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
    /// gets valuable under accessor if accessor passed
    static const Valuable* accessor_cast(const Valuable& mayBeAccessor);

protected:
    virtual Valuable* Clone() const;
    virtual int getTypeSize() const;

    template<class T>
    static const T* cast(const Valuable& v)
    {
        auto e = v.exp;
        while (e && e->exp) e = e->exp;
        auto t = dynamic_cast<const T*>(e ? e.get() : &v);
        if (!t)
        {
            auto a = accessor_cast(v);
            if (a)
                t = cast<T>(*a);
        }
        return t;
    }
    
    template<class T> Valuable() {}
    
    Valuable(ValuableDescendantMarker)
    {}
    
    Valuable(const Valuable& v, ValuableDescendantMarker)
    : hash(v.hash)
    , maxVaExp(v.maxVaExp)
    {}
    
    virtual std::ostream& print(std::ostream& out) const;
    virtual Valuable& Become(Valuable&& i);
    
    size_t hash = 0;
    int maxVaExp = 0; // ordering weight: vars max exponentiation in this valuable
    std::function<Valuable()> cachedFreeMember = [this]()->Valuable{
        auto c = calcFreeMember();
        this->cachedFreeMember = [c](){return c;};
        return c;
    };
    
public:

    enum View
    {
        None,
        Condensed,
        Flat
    };

    static thread_local bool optimizations;

    explicit Valuable(Valuable* v);
    operator std::type_index() const;
    
    Valuable& operator =(const Valuable& v);
    Valuable& operator =(const Valuable&& v);
    Valuable(const Valuable& v);
    Valuable(Valuable&& v);
    Valuable(double d);
    Valuable(int i = 0);
    Valuable(const long i);
    Valuable(unsigned long i);
    Valuable(unsigned long long i);
    
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
    virtual bool operator<(const Valuable& smarter) const;
    virtual bool operator==(const Valuable& smarter) const;
    virtual void optimize(); /// if it simplifies than it should become the type
    virtual void SetView(View v) { view = v; }

    // identify
    virtual bool IsInt() const;
    virtual bool IsFraction() const;
    virtual bool IsFormula() const;
    virtual bool IsExponentiation() const;
    virtual bool IsVa() const;
    virtual bool IsProduct() const;
    virtual bool IsSum() const;

    virtual Valuable abs() const;
    virtual Valuable sqrt() const;
    virtual Valuable calcFreeMember() const;    
    
    friend std::ostream& operator<<(std::ostream& out, const Valuable& obj);
    
    virtual const Variable* FindVa() const;
    virtual void CollectVa(std::set<Variable>& s) const;
    virtual void Eval(const Variable& va, const Valuable& v);
    virtual bool IsComesBefore(const Valuable& v) const; /// accepts same type as param
    
    bool Same(const Valuable& v) const;
    bool OfSameType(const Valuable& v) const;
    bool HasSameVars(const Valuable& v) const;
    bool IsMonicPolynomial() const;

    virtual int getMaxVaExp() const;
    using vars_cont_t = std::map<Variable, Valuable>;
    virtual const vars_cont_t& getCommonVars() const;
    static const vars_cont_t& emptyCommonVars();
    Valuable varless() const;
    static Valuable VaVal(const vars_cont_t& v);
    Valuable getVaVal() const;

    explicit operator bool() const;
    virtual explicit operator int() const;
    virtual explicit operator size_t() const;
    virtual explicit operator double() const;
    virtual explicit operator long double() const;
    virtual explicit operator unsigned char() const;
    
    size_t Hash() const;
    std::string str() const;

protected:
    View view = View::Flat;
};

    template<>
    const Accessor* Valuable::cast(const Valuable& v);
    
    size_t hash_value(const omnn::math::Valuable& v);
    
    void implement();
}}

namespace std
{
	omnn::math::Valuable abs(const omnn::math::Valuable& v);
	omnn::math::Valuable sqrt(const omnn::math::Valuable& v);
    
    template<>
    struct hash<omnn::math::Valuable> {
        size_t operator()(const omnn::math::Valuable& v) const {
            return v.Hash();
        }
    };
}

::omnn::math::Valuable operator"" _v(unsigned long long v);
::omnn::math::Valuable operator"" _v(long double v);

#define IMPLEMENT { implement(); throw; }

