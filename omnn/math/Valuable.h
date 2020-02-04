//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include <omnn/math/OpenOps.h>

#include <deque>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_set>

//#include <boost/shared_ptr.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/rational.hpp>

#define IMPLEMENT { implement(__FILE__ ":" BOOST_PP_STRINGIZE(__LINE__) " "); throw; } // " " __FUNCTION__


namespace omnn{
namespace math {
    class Valuable;
    size_t hash_value(const omnn::math::Valuable& v);
}
}

namespace std
{
    omnn::math::Valuable abs(const omnn::math::Valuable& v);
    omnn::math::Valuable sqrt(const omnn::math::Valuable& v);
    
    template<>
    struct hash<omnn::math::Valuable> {
        size_t operator()(const omnn::math::Valuable& v) const {
            return hash_value(v);
        }
    };
}

namespace omnn{
namespace math {
    
    using a_int = boost::multiprecision::cpp_int;
    using max_exp_t = boost::rational<a_int>;
    namespace ptrs = ::std;

    class VarHost;
    class Variable;
    struct ValuableDescendantMarker {};

class Valuable
        : public OpenOps<Valuable>
{
    using self = Valuable;
    using encapsulated_instance = ptrs::shared_ptr<Valuable>;
    encapsulated_instance exp = nullptr;

    static const a_int a_int_cz;
    static const max_exp_t max_exp_cz;

protected:
    const encapsulated_instance& getInst() const { return exp; }
    Valuable Link();
    virtual Valuable* Clone() const;
    virtual Valuable* Move();
    virtual void New(void*, Valuable&&);
    virtual int getTypeSize() const;
    virtual size_t getAllocSize() const;
    virtual void setAllocSize(size_t sz);
    const Valuable& get() const { return exp ? exp->get() : *this; }
    Valuable& get() { return exp ? exp->get() : *this; }
    
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
    
    template<class T>
    Valuable() {}
    
    Valuable(ValuableDescendantMarker)
    {}
    
    Valuable(const Valuable& v, ValuableDescendantMarker);
    
    Valuable(Valuable&& v, ValuableDescendantMarker);
    
    virtual std::ostream& print(std::ostream& out) const;
    virtual std::ostream& code(std::ostream& out) const;
    
    virtual Valuable& Become(Valuable&& i);
    
    size_t hash = 0;
    size_t sz = sizeof(Valuable);
    static constexpr a_int const& a_int_z = a_int_cz;
    static constexpr max_exp_t const& max_exp_z = max_exp_cz;
    max_exp_t maxVaExp;// = 0;//max_exp_z; // ordering weight: vars max exponentiation in this valuable
    
    std::function<Valuable()> DefaultCachedFn() {
        return [this]()->Valuable{
            auto c = calcFreeMember();
            this->cachedFreeMember = [c](){return c;};
            return c;
        };
    }
    std::function<Valuable()> cachedFreeMember = DefaultCachedFn();

public:

    enum View
    {
        None,
        Calc,
        Condensed,
        Equation,
        Flat,
        Solving,
    };
    
    enum class YesNoMaybe : uint8_t {
        Yes = 0b1, Maybe = 0b10, No = 0b100
    };

    template<class T>
    const T& as() const {
        auto& the = get();
        assert(the.Is<T>());
        return static_cast<const T&>(the);
    }

    template<class T>
    T& as() {
        auto& the = get();
        assert(the.Is<T>());
        return static_cast<T&>(the);
    }

//    friend operator bool(YesNoMaybe _) { return _==YesNoMaybe::Yes; }
    friend bool operator!(YesNoMaybe _) { return _ == YesNoMaybe::No; }
    friend YesNoMaybe operator||(YesNoMaybe, YesNoMaybe);
    friend YesNoMaybe operator&&(YesNoMaybe, YesNoMaybe);

    static thread_local bool optimizations;
    static thread_local bool bit_operation_optimizations;
    static thread_local bool enforce_solve_using_rational_root_test_only;

    explicit Valuable(Valuable* v);
    operator std::type_index() const;
    
    Valuable& operator =(const Valuable& v);
    Valuable& operator =(Valuable&& v);
    
    
    Valuable(const Valuable& v);

    template<class T,
            typename = typename std::enable_if<std::is_convertible<T&, Valuable&>::value>::type>
    Valuable(const T&& t)
    : exp(t.Clone())
    {
    }
     
    template<class T,
        typename = typename std::enable_if<
            !std::is_const<T>::value &&
            std::is_base_of<Valuable, T>::value
        >::type
    >
    Valuable(T&& t)
    : exp(t.Move())
    { }

    Valuable(Valuable&&) = default;
    Valuable();
    Valuable(double d);

    template<class T,
        typename = typename std::enable_if<!std::is_rvalue_reference<T>::value && std::is_integral<T>::value>::type
    >
    constexpr Valuable(T i = 0) : Valuable(a_int(i)) {}

    Valuable(const a_int&);
    Valuable(a_int&&);
    Valuable(boost::rational<a_int>&&);
    
    using NewVaFn_t = std::function<Valuable(const std::string&)>;
    Valuable(const std::string& s, NewVaFn_t newVa);
    Valuable(const std::string&, std::shared_ptr<VarHost>);

    virtual ~Valuable();
    virtual Valuable operator -() const;
    virtual Valuable& operator +=(const Valuable&);
    virtual Valuable& operator +=(int);
    virtual Valuable& operator *=(const Valuable&);
    virtual bool MultiplyIfSimplifiable(const Valuable& v);
    virtual std::pair<bool,Valuable> IsMultiplicationSimplifiable(const Valuable& v) const;
    virtual bool SumIfSimplifiable(const Valuable& v);
    virtual std::pair<bool,Valuable> IsSumationSimplifiable(const Valuable& v) const;
    virtual Valuable& operator /=(const Valuable&);
    virtual Valuable& operator %=(const Valuable&);
    virtual Valuable& operator--();
    virtual Valuable& operator++();
    virtual Valuable& operator^=(const Valuable&);
    virtual Valuable& d(const Variable& x);
    virtual Valuable I(const Variable& x, const Variable& C) const;
    virtual Valuable& i(const Variable& x, const Variable& C);
    virtual bool operator<(const Valuable&) const;
    virtual bool operator==(const Valuable&) const;
    virtual void optimize(); /// if it simplifies than it should become the type
    View GetView() const;
    void SetView(View v);

    // identify
    virtual bool IsInt() const;
    virtual bool IsFraction() const;
    virtual bool IsSimpleFraction() const;
    bool IsSimple() const;
    virtual bool IsFormula() const;
    virtual bool IsExponentiation() const;
    virtual bool IsVa() const;
    virtual bool IsVaExp() const;
    virtual bool IsProduct() const;
    virtual bool IsSum() const;
    virtual bool IsInfinity() const;
    virtual bool IsMInfinity() const;
    virtual bool Is_e() const;
    virtual bool Is_i() const;
    virtual bool Is_pi() const;
    virtual YesNoMaybe IsEven() const;
    virtual YesNoMaybe IsMultival() const;
    virtual void Values(const std::function<bool(const Valuable&)>&) const;

    virtual bool is(const std::type_index&) const;

    template<class T>
    bool Is() const {
        return exp ? exp->Is<T>() : is(typeid(T));
    }

    virtual a_int Complexity() const;
    virtual Valuable& sq();

    Valuable Sq() const;
    virtual Valuable abs() const;
    virtual Valuable Sqrt() const;
    virtual Valuable calcFreeMember() const;
    
    using solutions_t = std::unordered_set<Valuable>;
    static Valuable MergeAnd(const Valuable& v1, const Valuable& v2);
    static Valuable MergeOr(const Valuable& v1, const Valuable& v2);
    explicit Valuable(const solutions_t&);
    virtual Valuable operator()(const Variable&) const;
    virtual Valuable operator()(const Variable&, const Valuable& augmentation) const;
    bool IsUnivariate() const;

    virtual void solve(const Variable& va, solutions_t&) const;
    solutions_t Solutions() const;
    solutions_t IntSolutions() const;
    solutions_t Solutions(const Variable& v) const;
    solutions_t IntSolutions(const Variable& v) const;
    Valuable::solutions_t GetIntegerSolution() const;
    virtual Valuable::solutions_t GetIntegerSolution(const Variable& va) const;
    bool Test(const Variable& va, const Valuable& v) const;
    
    using extrenum_t = std::pair<Valuable, // extrema: value = x for f'(x) == 0, points where function is changing direction
                            std::pair<  Valuable,// from direction = g''(x), g(x)=f(-x), has previous direction sign
                                        Valuable // to direction = f''(x), has new direction sign
                                        >
                        >;

    using zone_t = std::pair<Valuable/*from*/,Valuable/*to*/>;
    using zero_zone_t = std::pair<zone_t/*whole*/,std::deque<zone_t>/*subranges*/>;
    zero_zone_t get_zeros_zones(const Variable& v, solutions_t& some) const;
    
    friend std::ostream& operator<<(std::ostream& out, const Valuable& obj);
    friend std::istream& operator>>(std::istream& in, const Valuable& obj);
    friend std::ostream& operator<<(std::ostream& out, const Valuable::solutions_t& obj);

    virtual max_exp_t getMaxVaExp()  const;
    using vars_cont_t = std::map<Variable, Valuable>;
    virtual const vars_cont_t& getCommonVars() const;
    virtual Valuable InCommonWith(const Valuable& v) const;
    static const vars_cont_t& emptyCommonVars();
    Valuable varless() const;
    static Valuable VaVal(const vars_cont_t& v);
    Valuable getVaVal() const;
    virtual bool eval(const std::map<Variable, Valuable>& with);
    
    virtual const Variable* FindVa() const;
    virtual bool HasVa(const Variable&) const;
    using var_set_t = std::set<Variable>;
    virtual void CollectVa(var_set_t& s) const;
    using va_names_t = std::map<std::string, Variable>;
    virtual void CollectVaNames(va_names_t& s) const;
    va_names_t VaNames() const;
    virtual std::shared_ptr<VarHost> getVaHost() const;
    
    var_set_t Vars() const;
    virtual void Eval(const Variable& va, const Valuable& v);
    virtual bool IsComesBefore(const Valuable& v) const; /// accepts same type as param
    
    bool Same(const Valuable& v) const;
    bool OfSameType(const Valuable& v) const;
    bool HasSameVars(const Valuable& v) const;
    bool IsMonic() const;

    Valuable(const std::string& s, const va_names_t& vaNames, bool itIsOptimized = false);
    Valuable(std::string_view str, const Valuable::va_names_t& vaNames, bool itIsOptimized);

    explicit operator bool() const;
    virtual explicit operator int() const;
    virtual explicit operator a_int() const;
    virtual explicit operator uint64_t() const;
    virtual explicit operator double() const;
    virtual explicit operator long double() const;
    virtual explicit operator uint32_t() const;
    virtual explicit operator unsigned char() const;
    virtual a_int& a();
    virtual const a_int& ca() const;
    
    // bits
    virtual Valuable bit(const Valuable& n) const;
    virtual Valuable bits(int n, int l) const;
    virtual Valuable Or(const Valuable& n, const Valuable& v) const;
    virtual Valuable And(const Valuable& n, const Valuable& v) const;
    virtual Valuable Xor(const Valuable& n, const Valuable& v) const;
    virtual Valuable Not(const Valuable& n) const;
    virtual Valuable& shl(const Valuable& n);
    virtual Valuable& shr(const Valuable& n);
    virtual Valuable& shr();
    virtual Valuable Shl(const Valuable& n) const;
    virtual Valuable Shr(const Valuable& n) const;
    virtual Valuable Shr() const;
    virtual Valuable sh(const Valuable& n) const;
    virtual Valuable Cyclic(const Valuable& total, const Valuable& shiftLeft) const;
    
    // logic
    Valuable Abet(const Variable& x, std::initializer_list<Valuable>) const;
    Valuable Equals(const Valuable& v) const;
    Valuable NotEquals(const Valuable& v) const;
//    Valuable NE(const Valuable& to, const Valuable& abet) const; // not equals
//    Valuable NE(const Variable& x, const Valuable& to, std::initializer_list<Valuable> abet) const; // not equals
    Valuable LogicAnd(const Valuable& v) const;
    Valuable LogicOr(const Valuable& v) const;
    Valuable& logic_or(const Valuable&); // inplace
    Valuable& logic_and(const Valuable&);
    
//    distinctZeros
//
//    distinct intersect
//
//    _1 = (x-1)(x-2),   _2 =(x-2)(x-3)
//
//    distinct union of (x-1)(x-2), (x-2)(x-3)   ==  (x-1)(x-2)(x-3)
//
//    (x-1)(x-2)   (x-1)                         (x-1)
//    ---------- = -----    =>  equation form :  ----- = 0    solve : x=1
//    (x-2)(x-3)   (x-3)                         (x-3)
//
//    _1 / solve = intersect:
//
//    (x-1)(x-2) / (x-1) == (x-2)
    
//    Valuable DistinctIntersect(const Valuable& with, const Variable& va){
//        auto toSolve = *this / with;
//        toSolve
//    }
    
    Valuable& intersect(const Valuable& with, const Variable& va){
        return logic_and(with);
    }
    Valuable Intersect(const Valuable& with, const Variable& va){
        return LogicAnd(with);
    }
    Valuable Ifz(const Valuable& Then, const Valuable& Else) const;
    Valuable IfEq(const Valuable& v, const Valuable& Then, const Valuable& Else) const;
    Valuable For(const Valuable& initialValue, const Valuable& lambda) const;


    size_t Hash() const;
    std::string str() const;
    virtual std::wstring save(const std::wstring&) const;

    virtual bool is_optimized() const;

protected:
    View view = View::Flat;
    bool optimized = false;
    void MarkAsOptimized();

    //   TODO : std::shared_ptr<std::vector<Valuable>> cachedValues;
};


Valuable implement(const char* str = "");

template <const unsigned long long I>
class vo {
    static const Valuable val;
public:
    constexpr operator const Valuable& () const {
        return val;
    }
    static const Valuable& get() { return val; }
};

template <const unsigned long long I>
const Valuable vo<I>::val = I;

}}

::omnn::math::Valuable operator"" _v(const char* v, std::size_t);
const ::omnn::math::Variable& operator"" _va(const char* v, std::size_t);
//constexpr 
::omnn::math::Valuable operator"" _v(unsigned long long v);
//constexpr const ::omnn::math::Valuable& operator"" _const(unsigned long long v);
::omnn::math::Valuable operator"" _v(long double v);


