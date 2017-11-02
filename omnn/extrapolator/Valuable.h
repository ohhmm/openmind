//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include "OpenOps.h"
#include <memory>
#include <set>
#include <type_traits>

namespace omnn{
namespace extrapolator {
    
    namespace ptrs = ::std;

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

protected:
    virtual Valuable* Clone() const;

    template<class T>
    static const T* cast(const Valuable& v)
    {
        auto e = v.exp;
        while (e && e->exp) e = e->exp;
        return dynamic_cast<const T*>(e ? e.get() : &v);
    }
    
    template<class T> Valuable() {}
    Valuable(ValuableDescendantMarker){}
    
    virtual std::ostream& print(std::ostream& out) const;
    virtual Valuable& Become(Valuable&& i);

public:
    explicit Valuable(Valuable* v);
    
    Valuable& operator =(const Valuable& v);
    Valuable& operator =(const Valuable&& v);
    Valuable(const Valuable& v);
    Valuable(Valuable&& v);
    Valuable(int i = 0);

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
    virtual bool operator<(const Valuable& number) const;
    virtual bool operator==(const Valuable& number) const;
    virtual Valuable abs() const;
    virtual void optimize(); /// if it simplifies than it should become the type
    virtual Valuable sqrt() const;
    friend std::ostream& operator<<(std::ostream& out, const Valuable& obj);
    
    virtual const Variable* FindVa() const;
    virtual void CollectVa(std::set<Variable>& s) const;
    virtual void Eval(const Variable& va, const Valuable& v);
    
    bool OfSameType(const Valuable& v) const;
    bool Same(const Valuable& v) const;
    
    virtual size_t Hash() const;
};

}}

namespace std
{
	omnn::extrapolator::Valuable abs(const omnn::extrapolator::Valuable& v);
	omnn::extrapolator::Valuable sqrt(const omnn::extrapolator::Valuable& v);
    
    template<>
    struct hash<omnn::extrapolator::Valuable> {
        size_t operator()(const omnn::extrapolator::Valuable& v) const {
            return v.Hash();
        }
    };
}

::omnn::extrapolator::Valuable operator"" _v(unsigned long long v);
