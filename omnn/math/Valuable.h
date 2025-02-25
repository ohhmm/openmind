//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include <omnn/math/OpenOps.h>
#include <omnn/math/YesNoMaybe.h>

#include <deque>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <typeindex>
#include <unordered_set>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/preprocessor.hpp>
#include <boost/rational.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/split_member.hpp>


#define _NUM2STR(x) #x
#define NUM2STR(x) _NUM2STR(x)
#define LINE_NUMBER_STR NUM2STR(__LINE__)

#define IMPLEMENT {                                                                                                    \
        ::omnn::math::implement(__FILE__ ":" LINE_NUMBER_STR " ");                                                                   \
        throw;                                                                                                         \
    }
#define LOG_AND_IMPLEMENT(Param) { \
    ::omnn::math::implement(((::std::stringstream&)(::std::stringstream() << __FILE__ ":" LINE_NUMBER_STR " " << Param)).str().c_str()); \
        throw;                                                                                                          \
    }


namespace omnn {
namespace math {
class Valuable;
class Variable;
class Integer;
class Exponentiation;
class Fraction;
class PrincipalSurd;
class Sum;
size_t hash_value(const omnn::math::Valuable&);

using vars_cont_t = std::map<Variable, Valuable>;
std::ostream& operator<<(std::ostream& out, const vars_cont_t& vars);

} // namespace math
} // namespace omnn

namespace std {
omnn::math::Valuable abs(const omnn::math::Valuable&);
omnn::math::Valuable log(const omnn::math::Valuable&);
omnn::math::Valuable pow(const omnn::math::Valuable&, const omnn::math::Valuable&);
omnn::math::Valuable sqrt(const omnn::math::Valuable&);
omnn::math::Valuable tanh(const omnn::math::Valuable&);

template <>
struct hash<omnn::math::Valuable> {
    [[nodiscard]]
    size_t operator()(const omnn::math::Valuable& valuable) const { return hash_value(valuable); }
};

template <>
struct equal_to<::omnn::math::Valuable> {
    [[nodiscard]]
    bool operator()(const ::omnn::math::Valuable&, const ::omnn::math::Valuable&) const;
};

} // namespace std

namespace omnn{
namespace math {

namespace constants {
extern const Valuable& e;
extern const Valuable& i;
extern const Valuable& zero;
extern const Valuable& one;
extern const Valuable& two;
extern const Valuable& half;
extern const Valuable& quarter;
extern const Valuable& minus_1;
extern const Valuable& plus_minus_1; // ±1
extern const Valuable& zero_or_1;
extern const Valuable& infinity;
extern const Valuable& minfinity;
extern const Valuable& pi;
extern const Variable& integration_result_constant;
}

    using a_int = boost::multiprecision::cpp_int;

    using a_rational = boost::multiprecision::cpp_rational;
    //using a_rational = boost::rational<a_int>;

    using max_exp_t = a_rational;
    namespace ptrs = ::std;

    class VarHost;
    struct ValuableDescendantMarker {};

    Valuable implement(const char* str = "");


class Valuable
        : public OpenOps<Valuable>
{
    using self = Valuable;

    static const a_int a_int_cz;
    static const max_exp_t max_exp_cz;

    typedef Valuable& (Valuable::*method_t)(const Valuable&);
    self& call_polymorphic_method(method_t, const self& arg);

protected:
    using encapsulated_instance = ptrs::shared_ptr<Valuable>;
    encapsulated_instance exp = nullptr;

    virtual bool IsSubObject(const Valuable& o) const;
    virtual Valuable* Clone() const;
    virtual Valuable* Move();
    virtual void New(void*, Valuable&&);
    static constexpr size_t DefaultAllocSize = 768;
    constexpr virtual size_t getTypeSize() const { return sizeof(Valuable); }
    constexpr virtual size_t getAllocSize() const { return sz; }
    constexpr virtual void setAllocSize(size_t sz) { this->sz = sz; }

    template<class T>
    constexpr Valuable() {}

    constexpr Valuable(ValuableDescendantMarker)
    {}

    Valuable(const Valuable& v, ValuableDescendantMarker);

    Valuable(Valuable&& v, ValuableDescendantMarker);

    virtual std::ostream& print(std::ostream& out) const;
    virtual std::wostream& print(std::wostream& out) const;

    Valuable& Become(Valuable&&);

    size_t hash = 0;
    size_t sz = sizeof(Valuable);
    static constexpr a_int const& a_int_z = a_int_cz;
    static constexpr max_exp_t const& max_exp_z = max_exp_cz;
    max_exp_t maxVaExp = 0;//max_exp_z; // ordering weight: vars max exponentiation in this valuable

    bool optimized = false;
    void MarkAsOptimized();

public:
    constexpr const encapsulated_instance& getInst() const { return exp; }
    void MarkNotOptimized();


    /// <summary>
    /// Depends on optimizing goals, the view may set different.
    // time to evaluate
	// time to differentiate
	// string lengths
	// memory allocation size
    // at the moment, well supported are Equation and Flat views
    /// </summary>
    enum class View
		: uint8_t
    {
        None = 0,
        Calc = 1,
        Condensed = 2,
        Equation = 4,
        Flat = 8,
		Fraction = 16,
        Solving = 32 | Equation,
        SupersetOfRoots = 64 | Solving
    };

    friend std::ostream& operator<<(std::ostream& os, View v) {
	return os << static_cast<uint8_t>(v);
    }

    friend constexpr View operator&(View a, View b) {
        return View(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }
    friend constexpr View operator|(View a, View b) {
        return View(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    const Valuable& get() const { return exp ? exp->get() : *this; }
    Valuable& get() { return exp ? exp->get() : *this; }

    template<class T>
    const T& as() const;

    template<class T>
    T& as() {
        auto& the = get();
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
        if (!the.Is<T>()) {
            ::omnn::math::implement(((::std::stringstream&)(::std::stringstream() << __FILE__ ":" LINE_NUMBER_STR " " << "Attempt to cast " << Type().name() << ' ' << *this << " to " << typeid(T).name())).str().c_str());
            throw;
        }
#endif
        return static_cast<T&>(the);
    }

    template <class T>
    const T* As() const {
        return Is<T>() ? &as<T>() : nullptr;
    }

    [[nodiscard]]
    virtual encapsulated_instance SharedFromThis();

    static thread_local bool optimizations;
    static thread_local bool bit_operation_optimizations;
    static thread_local bool enforce_solve_using_rational_root_test_only;

    class [[maybe_unused]] OptimizeOn {
        bool opts;
    public:
#ifdef MSVC
        constexpr
#endif
        OptimizeOn() : opts(optimizations) {
            optimizations = true;
        }
        ~OptimizeOn(){
            optimizations = opts;
        }
    };

    class [[maybe_unused]] OptimizeOff {
        bool opts;
    public:
#ifdef MSVC
        constexpr
#endif
        OptimizeOff() : opts(optimizations) {
			optimizations = {};
        }
        ~OptimizeOff(){
            optimizations = opts;
        }
    };

    class [[maybe_unused]] ViewOptimizePause {
        View view;
        Valuable* valuable;
    public:
        ViewOptimizePause(Valuable* value)
            : view(value->GetView())
            , valuable(value)
        {
            value->SetView(View::Calc);
        }
        ~ViewOptimizePause() {
            valuable->SetView(view);
        }
    };

    explicit Valuable(Valuable* v);
    explicit Valuable(encapsulated_instance&&);
    explicit Valuable(const encapsulated_instance&);
    virtual std::type_index Type() const;
    const Valuable Link() const; // TODO : handle simulteneous values changes

    Valuable& operator =(const Valuable&);
    Valuable& operator =(Valuable&&);

    template <typename ValuableT>
        requires(std::derived_from<ValuableT, Valuable>)
    Valuable& operator=(ptrs::shared_ptr<ValuableT>&& ptr) {
        return operator=(Valuable(std::move(std::static_pointer_cast<Valuable>(ptr))));
    }

    bool SerializedStrEqual(const std::string_view& s) const;

    Valuable(const Valuable&);

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

#ifdef MSVC
    constexpr
#endif
    Valuable(Valuable&&) = default;
    Valuable();
    Valuable(double d);

    template<class T,
        typename = typename std::enable_if<!std::is_rvalue_reference<T>::value && std::is_integral<T>::value>::type
    >
    constexpr Valuable(T i = 0) : Valuable(a_int(i)) {}

    Valuable(const a_int&);
    Valuable(a_int&&);
    Valuable(a_rational&&);
    Valuable(const a_rational&);

    using NewVaFn_t = std::function<Valuable(const std::string&)>;
    Valuable(const std::string& s, NewVaFn_t newVa);
    Valuable(const std::string_view&, std::shared_ptr<VarHost> host = {}, bool itIsOptimized = {});

    static void DispatchDispose(encapsulated_instance&&);
    //constexpr
	virtual ~Valuable()//{}
        ;
    virtual Valuable operator -() const;
    virtual Valuable& operator +=(const Valuable&);
    virtual Valuable& operator +=(int);
    virtual Valuable& operator *=(const Valuable&);
    virtual bool MultiplyIfSimplifiable(const Valuable&);
    virtual bool MultiplyIfSimplifiable(const Integer& i);
    virtual std::pair<bool,Valuable> IsMultiplicationSimplifiable(const Valuable&) const;
    virtual bool SumIfSimplifiable(const Valuable&);
    virtual std::pair<bool,Valuable> IsSummationSimplifiable(const Valuable&) const;
    virtual std::pair<bool,Valuable> IsModSimplifiable(const Valuable&) const;
    virtual Valuable& operator /=(const Valuable&);
    virtual Valuable& operator %=(const Valuable&);
    virtual Valuable& operator--();
    virtual Valuable& operator++();
    virtual Valuable& operator^=(const Valuable&);

    // returns the greatest common divisor (GCD) with the given object
    virtual Valuable GCD(const Valuable&) const;
    virtual Valuable& gcd(const Valuable&);

    // returns the least common multiple (LCM) with the given object
    virtual Valuable LCM(const Valuable&) const;
    virtual Valuable& lcm(const Valuable&);

    virtual Valuable& d(const Variable& x);
    struct IntegrationParams {
        const Valuable& from = constants::minfinity;
        const Valuable& to = constants::infinity;
        const Variable& C = constants::integration_result_constant;
    };
    void integral(); // expects to be single-variable
    virtual Valuable& integral(const Variable& x, const Variable& C);
    Valuable& integral(const Variable& x) { return integral(x, constants::integration_result_constant); }
    Valuable& integral(const Variable& x, const Valuable& from, const Valuable& to, const Variable& C);
    Valuable Integral(const Variable& x, const Variable& C = constants::integration_result_constant) const;
    Valuable Integral(const Variable& x,
		const Valuable& from = constants::minfinity,
		const Valuable& to = constants::infinity,
                      const Variable& C = constants::integration_result_constant) const;

    virtual bool operator<(const Valuable&) const;
    virtual bool operator==(const Valuable&) const;
    virtual void optimize(); /// if it simplifies than it should become the type
    View GetView() const;
    void SetView(View v);
#if defined(MSVC) || defined(__APPLE__)
    constexpr
#endif
    bool IsEquation() const {
        return (GetView() & View::Equation) != View::None;
    }

    // identify
    virtual bool IsConstant() const;
    virtual bool IsInt() const;
    virtual bool IsFraction() const;
    virtual bool IsRadical() const;
    virtual bool IsPrincipalSurd() const;
    virtual bool IsSurd() const;
    virtual bool IsSimpleFraction() const;
    virtual bool IsFormula() const;
    virtual bool IsExponentiation() const;
    virtual bool IsLogarithm() const;
    virtual bool IsLimit() const;
    virtual bool IsVa() const;
    virtual bool IsVaExp() const;
    virtual bool IsProduct() const;
    virtual bool IsSum() const;
    virtual bool IsInfinity() const;
    virtual bool IsModulo() const;
    virtual bool IsMInfinity() const;
    virtual bool IsNaN() const;
    virtual bool Is_e() const;
    virtual bool Is_i() const;
    virtual bool Is_pi() const;
    virtual bool IsZero() const;

    virtual bool IsSimple() const;
    virtual YesNoMaybe IsRational() const;
    virtual YesNoMaybe IsEven() const;
    virtual YesNoMaybe IsMultival() const;
    virtual void Values(const std::function<bool(const Valuable&)>&) const; /// split multival to distinct values and return those using visitor pattern

    virtual bool is(const std::type_index&) const;

    template<class T>
    bool Is() const {
        return get().is(typeid(T));
    }

    virtual a_int Complexity() const;
    virtual Valuable& sq();
    [[nodiscard]]
    virtual Valuable Sq() const;
    [[nodiscard]]
    virtual Valuable Sign() const;
    [[nodiscard]]
    virtual Valuable abs() const;
    [[nodiscard]]
    virtual Valuable Abs() const;
    [[nodiscard]]
    virtual Valuable Cos() const;
    [[nodiscard]]
    virtual Valuable Sin() const;
    [[nodiscard]]
    virtual Valuable Sqrt() const;
    virtual Valuable& sqrt();
    [[nodiscard]]
    virtual Valuable Tg() const;
    [[nodiscard]]
    virtual Valuable Tanh() const;
    virtual void gamma(); // https://en.wikipedia.org/wiki/Gamma_function
    [[nodiscard]]
    virtual Valuable Gamma() const;
    virtual Valuable& factorial();
    [[nodiscard]]
    virtual Valuable Factorial() const;
    Valuable Factors(const Variable& factor) const;
    [[nodiscard]]
    virtual Valuable FirstFactor() const;
    [[nodiscard]]
    Valuable LastFactor() const;

    [[nodiscard]]
    virtual Valuable calcFreeMember() const;

    virtual Valuable& reciprocal();
    [[nodiscard]]
    virtual Valuable Reciprocal() const;

    using expressions_t = std::unordered_set<Valuable>;
    using solutions_t = expressions_t;
    static Valuable MergeAnd(const Valuable&, const Valuable&); /// conjunctive merge of two values
    static Valuable MergeOr(const Valuable&, const Valuable&); /// disjunctive merge algorithm deduced from square equation solutions formula, works for integers
    static Valuable MergeOr(const Valuable&, const Valuable&, const Valuable&); /// algorithm is to be deduced from cubic equation solutions formula
    static Valuable MergeOr(const Valuable&, const Valuable&, const Valuable&, const Valuable&); /// double merge
    explicit Valuable(solutions_t&&);
    virtual Valuable operator()(const Variable&) const;
    virtual Valuable operator()(const Variable&, const Valuable& augmentation) const;
    [[nodiscard]]
    bool IsUnivariable() const;

    virtual void solve(const Variable& va, solutions_t&) const;
    solutions_t solve(const Variable&) const;
    virtual solutions_t Distinct() const;
    virtual Valuable Univariate() const;
    virtual bool IsPolynomial(const Variable&) const;
    virtual size_t FillPolynomialCoefficients(std::vector<Valuable>& coefficients, const Variable& v) const;
    std::vector<Valuable> Coefficients(const Variable&) const;

    solutions_t Solutions() const;
    solutions_t IntSolutions() const;
    solutions_t Solutions(const Variable&) const;
    solutions_t IntSolutions(const Variable&) const;
    solutions_t GetIntegerSolution() const;
    virtual solutions_t GetIntegerSolution(const Variable& va) const;
    bool Test(const Variable& va, const Valuable&) const;

    // FIXME : waiting for virtual template method https://github.com/ohhmm/llvm-project/pull/1
    //virtual template <typename T>
    //const T* Divisor() const {
    //    if (exp)
    //        return exp->Divisor<T>();
    //    else
    //        IMPLEMENT
    //}
    virtual const PrincipalSurd* PrincipalSurdFactor() const;

    virtual Valuable SumOfRoots() const;
    virtual Valuable ProductOfRoots() const;

    using extrenum_t = std::pair<Valuable, // extrema: value = x for f'(x) == 0, points where function is changing direction
                            std::pair<  Valuable,// from direction = g''(x), g(x)=f(-x), has previous direction sign
                                        Valuable // to direction = f''(x), has new direction sign
                                        >
                        >;

    using zone_t = std::pair<Valuable/*from*/,Valuable/*to*/>;
    using ranges_t = std::pair<zone_t/*whole*/,std::deque<zone_t>/*subranges*/>;

    using va_names_t = std::set<std::string>;
    virtual void CollectVaNames(va_names_t&) const;
    virtual void CollectVa(std::set<Variable>&) const;
    virtual const Variable* FindVa() const;
    virtual bool HasVa(const Variable&) const;
    virtual bool eval(const std::map<Variable, Valuable>& with);
    virtual void Eval(const Variable&, const Valuable&);

    using universal_lambda_params_t = const std::vector<double>&;
    using universal_lambda_t = std::function<Valuable(universal_lambda_params_t)>;
    using variables_for_lambda_t = std::vector<Variable>;
    virtual universal_lambda_t CompileIntoLambda(variables_for_lambda_t) const;

    virtual bool IsComesBefore(const Valuable&) const;
    virtual bool OfSameType(const Valuable&) const;
    virtual bool Same(const Valuable&) const;
    virtual Valuable InCommonWith(const Valuable&) const;
    virtual max_exp_t getMaxVaExp() const;

    virtual operator a_rational() const;
    virtual operator double() const;
    virtual operator long double() const;

    [[nodiscard]]
    size_t Hash() const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        boost::serialization::split_member(ar, *this, version);
    }

    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar & hash;
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        ar & hash;
    }
};

template<class T>
const T& Valuable::as() const {
    auto& the = get();
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
    if (!the.Is<T>()) {
        ::omnn::math::implement(((::std::stringstream&)(::std::stringstream() << __FILE__ ":" LINE_NUMBER_STR " " << "Attempt to cast " << Type().name() << ' ' << *this << " to " << typeid(T).name())).str().c_str());
        throw;
    }
#endif
    return static_cast<const T&>(the);
}

template <const long long I>
class vo {
    static const Valuable val;
public:
    constexpr operator const Valuable& () const {
        return val;
    }
    static const Valuable& get() { return val; }
};

template <const long long I>
const Valuable vo<I>::val = I;

#if defined(MSVC) || defined(__APPLE__)
template <const double I>
class vf {
    static const Valuable val;
public:
    constexpr operator const Valuable& () const {
        return val;
    }
    static const Valuable& get() { return val; }
};

template <const double I>
const Valuable vf<I>::val = I;
#endif


} // namespace math
} // namespace omnn

namespace std {

template <class T>
    requires std::derived_from<T, ::omnn::math::Valuable>
struct hash<T> {
    [[nodiscard]]
    constexpr size_t operator()(const T& valuable) const { return static_cast<const omnn::math::Valuable&>(valuable).Hash(); }
};

} // namespace std

::omnn::math::Valuable operator"" _v(const char*, std::size_t);
const ::omnn::math::Variable& operator"" _va(const char*, std::size_t);
//constexpr
::omnn::math::Valuable operator"" _v(unsigned long long);
//constexpr const ::omnn::math::Valuable& operator"" _const(unsigned long long);
::omnn::math::Valuable operator"" _v(long double);
