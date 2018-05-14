//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include "OpenOps.h"
#include <deque>
#include <map>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_set>
#include <boost/shared_ptr.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#define IMPLEMENT { implement(); throw; }

namespace omnn{
namespace math {
    
    using a_int = boost::multiprecision::cpp_int;
    namespace ptrs = ::std;

    class Variable;
    struct ValuableDescendantMarker {};
    
    void implement();

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
    virtual Valuable* Move();
    virtual void New(void*, Valuable&&);
    virtual int getTypeSize() const;
    virtual int getAllocSize() const;
    virtual void setAllocSize(int sz);
    
    template<class T>
    static const T* cast(const Valuable& v)
    {
        auto e = v.exp;
        while (e && e->exp) e = e->exp;
        auto t = dynamic_cast<const T*>(e ? e.get() : &v);
        return t;
    }
    
    template<class T>
    static T* cast(Valuable& v)
    {
        auto e = v.exp;
        while (e && e->exp) e = e->exp;
        auto t = dynamic_cast<T*>(e ? e.get() : &v);
        return t;
    }
    
    template<class T> Valuable() {}
    
    Valuable(ValuableDescendantMarker)
    {}
    
    Valuable(const Valuable& v, ValuableDescendantMarker)
    : hash(v.Hash())
    , maxVaExp(v.getMaxVaExp())
    , optimized(v.optimized)
    {}
    
    Valuable(Valuable&& v, ValuableDescendantMarker)
    : hash(v.Hash())
    , maxVaExp(v.getMaxVaExp())
    , optimized(v.optimized)
    {
        
    }
    
    virtual std::ostream& print(std::ostream& out) const;
    virtual std::ostream& code(std::ostream& out) const;
    
    virtual Valuable& Become(Valuable&& i);
    
    size_t hash = 0;
    size_t sz = sizeof(Valuable);
    a_int maxVaExp = 0; // ordering weight: vars max exponentiation in this valuable
    
    static std::function<Valuable()> DefaultCachedFm();
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
        Equation,
        Flat,
        Solving,
    };

    static thread_local bool optimizations;

    explicit Valuable(Valuable* v);
    operator std::type_index() const;
    
    Valuable& operator =(const Valuable& v);
    Valuable& operator =(Valuable&& v);
    
    
    Valuable(const Valuable& v);

    template<class T,
            typename = typename std::enable_if<std::is_convertible<T&, Valuable&>::value>::type>
    Valuable(const T&& t)
    : exp(t.Clone())
    , hash(t.Hash())
    , maxVaExp(t.getMaxVaExp())
    , optimized(t.optimized)
    {
    }
     
    template<class T,
        typename = typename std::enable_if<
                std::is_rvalue_reference<T>::value &&
                !std::is_const<T>::value>::type
    >
    Valuable(T t)
    : exp(t.Move())
    , hash(t.Hash())
    , maxVaExp(t.getMaxVaExp())
    , optimized(t.optimized)
    { }

    Valuable(Valuable&&) = default;
    
    Valuable(double d);
    Valuable(int i = 0);
    Valuable(const a_int&);
    Valuable(a_int&&);
    Valuable(const long);
    Valuable(unsigned);
    Valuable(unsigned long);
    Valuable(unsigned long long);
    
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
    virtual Valuable& d(const Variable& x);
//    virtual Valuable& I(const Variable& x);
    virtual bool operator<(const Valuable&) const;
    virtual bool operator==(const Valuable&) const;
    virtual void optimize(); /// if it simplifies than it should become the type
    virtual void SetView(View v);

    // identify
    virtual bool IsInt() const;
    virtual bool IsFraction() const;
    virtual bool IsSimpleFraction() const;
    virtual bool IsFormula() const;
    virtual bool IsExponentiation() const;
    virtual bool IsVa() const;
    virtual bool IsProduct() const;
    virtual bool IsSum() const;
    virtual bool IsInfinity() const;
    virtual bool IsMInfinity() const;

    virtual Valuable abs() const;
    virtual Valuable sqrt() const;
    virtual Valuable calcFreeMember() const;
    
    using solutions_t = std::unordered_set<Valuable>; // do not use std::set<Valuable> until it tested. it may skip unequal items
    virtual solutions_t operator()(const Variable&) const;
    virtual solutions_t operator()(const Variable&, const Valuable& augmentation) const;
    bool IsUnivariate() const;
    
    virtual void solve(const Variable& va, solutions_t&) const;
    solutions_t solutions() const;
    solutions_t solutions(const Variable& v) const;
    Valuable::solutions_t GetIntegerSolution() const;
    virtual Valuable::solutions_t GetIntegerSolution(const Variable& va) const;
    
    using extrenums_t = std::vector<std::pair<Valuable/*value*/,Valuable/*direction*/> >;
    extrenums_t extrenums() const;
    extrenums_t extrenums(const Variable& v) const;
    extrenums_t& extrenums(const Variable& v, extrenums_t& extrs) const;

    using zone_t = std::pair<Valuable/*from*/,Valuable/*to*/>;
    using zero_zone_t = std::pair<zone_t/*whole*/,std::deque<zone_t>/*subranges*/>;
    zero_zone_t get_zeros_zones(const Variable& v) const;
    zero_zone_t get_zeros_zones(const Variable& v, const extrenums_t&) const;
    
    friend std::ostream& operator<<(std::ostream& out, const Valuable& obj);
    
    virtual const Variable* FindVa() const;
    virtual bool HasVa(const Variable&) const;
    using var_set_t = std::set<Variable>;
    virtual void CollectVa(var_set_t& s) const;
    var_set_t Vars() const;
    virtual void Eval(const Variable& va, const Valuable& v);
    virtual bool IsComesBefore(const Valuable& v) const; /// accepts same type as param
    
    bool Same(const Valuable& v) const;
    bool OfSameType(const Valuable& v) const;
    bool HasSameVars(const Valuable& v) const;
    bool IsMonic() const;

    virtual a_int getMaxVaExp() const;
    using vars_cont_t = std::map<Variable, Valuable>;
    virtual const vars_cont_t& getCommonVars() const;
    static const vars_cont_t& emptyCommonVars();
    Valuable varless() const;
    static Valuable VaVal(const vars_cont_t& v);
    Valuable getVaVal() const;

    explicit operator bool() const;
    virtual explicit operator int() const;
    virtual explicit operator a_int() const;
    virtual explicit operator size_t() const;
    virtual explicit operator double() const;
    virtual explicit operator long double() const;
    virtual explicit operator unsigned() const;
    virtual explicit operator unsigned char() const;
    virtual a_int& a();
    virtual const a_int& ca() const;
    
//    virtual Valuable ifz(const Valuable& z, const Valuable& Then, const Valuable& Else) const;
//    Valuable For(const Valuable& initialValue, const Valuable& lambda) const
//    {
//
//    }
    virtual Valuable bit(const Valuable& n) const;
    virtual Valuable bits(int n, int l) const;
    virtual Valuable Or(const Valuable& n, const Valuable& v) const;
    virtual Valuable And(const Valuable& n, const Valuable& v) const;
    virtual Valuable Xor(const Valuable& n, const Valuable& v) const;
    virtual Valuable Not(const Valuable& n) const;
    virtual Valuable& shl(const Valuable& n);
    virtual Valuable Shl(const Valuable& n) const;
    virtual Valuable Shr(const Valuable& n) const;
    virtual Valuable Shr() const;
    virtual Valuable sh(const Valuable& n) const;
    virtual Valuable Cyclic(const Valuable& total, const Valuable& shiftLeft) const;
    
    size_t Hash() const;
    std::string str() const;
    virtual std::wstring save(const std::wstring&) const;

protected:
    View view = View::Flat;
    bool optimized {};
};

    size_t hash_value(const omnn::math::Valuable& v);
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

::omnn::math::Valuable operator"" _v(const char* v, std::size_t);
::omnn::math::Valuable operator"" _v(unsigned long long v);
::omnn::math::Valuable operator"" _v(long double v);


