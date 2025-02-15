//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once

// Platform macros must be defined before any other includes
#include <omnn/math/Platform.h>

// Other includes
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

    using method_t = Valuable& (Valuable::*)(const Valuable&);
    self& call_polymorphic_method(method_t method, const self& arg);

protected:
    using encapsulated_instance = ptrs::shared_ptr<Valuable>;
    encapsulated_instance exp = nullptr;

    virtual bool IsSubObject(const Valuable& o) const;
    virtual Valuable* Clone() const;
    virtual Valuable* Move();
    virtual void New(void*, Valuable&&);
    static constexpr size_t DefaultAllocSize = 768;
    virtual size_t getTypeSize() const noexcept { return sizeof(Valuable); }
    virtual size_t getAllocSize() const noexcept { return sz; }
    virtual void setAllocSize(size_t sz) noexcept { this->sz = sz; }

    template<class T>
    Valuable() noexcept {}

    Valuable(ValuableDescendantMarker) noexcept
    {}

    Valuable(const Valuable& v, ValuableDescendantMarker);

    Valuable(Valuable&& v, ValuableDescendantMarker);

    virtual std::ostream& print(std::ostream& out) const;
    virtual std::wostream& print(std::wostream& out) const;

    Valuable& Become(Valuable&&);

    size_t hash = 0;
    size_t sz = sizeof(Valuable);
    static const a_int& a_int_z;
    static const max_exp_t& max_exp_z;
    View view = View::None;
    max_exp_t maxVaExp = 0;//max_exp_z; // ordering weight: vars max exponentiation in this valuable

    bool optimized = false;
    void MarkAsOptimized();

public:
    const encapsulated_instance& getInst() const noexcept { return exp; }
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
            LOG_AND_IMPLEMENT("Attempt to cast " << Type().name() << ' ' << *this << " to " << typeid(T).name());
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

    Valuable(Valuable&&) noexcept = default;
    Valuable();
    Valuable(double d);

    template<class T,
        typename = typename std::enable_if<!std::is_rvalue_reference<T>::value && std::is_integral<T>::value>::type
    >
    Valuable(T i = 0) noexcept : Valuable(a_int(i)) {}

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
    View GetView() const noexcept { return view; }
    void SetView(View v) noexcept { view = v; }
    bool IsEquation() const noexcept {
        return (view & View::Equation) != View::None;
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
    ranges_t get_zeros_zones(const Variable& v, solutions_t& some) const;

    friend std::ostream& operator<<(std::ostream& out, const Valuable& obj);
    friend std::wostream& operator<<(std::wostream& out, const Valuable& obj);
    friend std::istream& operator>>(std::istream& in, const Valuable& obj);
    friend std::ostream& operator<<(std::ostream& out, const Valuable::solutions_t& obj);

    virtual max_exp_t getMaxVaExp()  const;
    using vars_cont_t = std::map<Variable, Valuable>;
    virtual vars_cont_t GetVaExps() const;
    virtual const vars_cont_t& getCommonVars() const;
    virtual vars_cont_t calcCommonVars() const;
    virtual Valuable InCommonWith(const Valuable&) const;
    static const vars_cont_t& emptyCommonVars();
    virtual Valuable varless() const;
    static Valuable VaVal(const vars_cont_t&);
    Valuable getVaVal() const;
    Valuable commonVarsWith(const Valuable&) const;
    virtual bool eval(const vars_cont_t& with);
    Valuable Eval(const vars_cont_t& with) const;

    // virtual template <class T> const Valuable* Find<T>() const;   // FIXME : check for virtual template methods in new C++ standards https://www.quora.com/profile/Serg-Kryvonos/Template-virtual-method-into-C
    virtual const Variable* FindVa() const;
    virtual bool HasVa(const Variable&) const;
    using var_set_t = std::set<Variable>;
    virtual void CollectVa(var_set_t& s) const;
    using va_names_t = std::map<std::string_view, Variable>;
    virtual void CollectVaNames(va_names_t& s) const;
    va_names_t VaNames() const;
    virtual std::shared_ptr<VarHost> getVaHost() const;

    var_set_t Vars() const;
    virtual void Eval(const Variable& va, const Valuable&);
    virtual bool IsComesBefore(const Valuable&) const; /// accepts same type as param

    virtual bool Same(const Valuable&) const;
    bool OfSameType(const Valuable&) const;
    bool HasSameVars(const Valuable&) const;
    bool IsMonic() const;

    Valuable(const std::string& s, const va_names_t& vaNames, bool itIsOptimized = false);
    Valuable(std::string_view str, const va_names_t& vaNames, bool itIsOptimized = false);

	Valuable operator!() const;
    explicit operator bool() const;
    virtual explicit operator int() const;
    virtual explicit operator a_int() const;
    virtual explicit operator uint64_t() const;
    virtual explicit operator double() const;
    virtual explicit operator long double() const;
    virtual explicit operator a_rational() const;
    virtual explicit operator uint32_t() const;
    virtual explicit operator unsigned char() const;
    virtual a_int& a();
    virtual const a_int& ca() const;

    // bits
    virtual Valuable bit(const Valuable& n = constants::zero) const;
    virtual Valuable bits(int n, int l) const;
    virtual Valuable Or(const Valuable& n, const Valuable&) const;
    virtual Valuable And(const Valuable& n, const Valuable&) const;
    virtual Valuable Xor(const Valuable& n, const Valuable&) const;
    virtual Valuable Not(const Valuable& n) const;
    virtual Valuable& shl();
    virtual Valuable& shl(const Valuable& n);
    virtual Valuable& shr(const Valuable& n);
    virtual Valuable& shr();
    virtual Valuable Shl() const;
    virtual Valuable Shl(const Valuable& n) const;
    virtual Valuable Shr(const Valuable& n) const;
    virtual Valuable Shr() const;
    virtual Valuable sh(const Valuable& n) const;
    virtual Valuable Cyclic(const Valuable& total, const Valuable& shiftLeft) const;

    // logic
    static Valuable Abet(const Variable& x, std::initializer_list<Valuable>);
    template <class Fwd>
    constexpr Valuable& equals(Fwd&& valuable) {
//        SetView(View::Equation);  // FIXME: see ifz test
        return operator -=(std::forward<Fwd>(valuable));
    }
    Valuable Equals(const Valuable&) const;
    Valuable NotEquals(const Valuable&) const;
//    Valuable NE(const Valuable& to, const Valuable& abet) const; // not equals
//    Valuable NE(const Variable& x, const Valuable& to, std::initializer_list<Valuable> abet) const; // not equals
    Valuable LogicAnd(const Valuable& valuable) const;
    Valuable operator&&(const Valuable& valuable) const { return LogicAnd(valuable); }
    Valuable LogicOr(const Valuable& valuable) const;
    Valuable operator||(const Valuable& valuable) const { return LogicOr(valuable); }
    Valuable& logic_or(const Valuable& valuable); // inplace
    Valuable& logic_and(const Valuable& valuable);

    Valuable Intersect(const Valuable& valuable) const;
    Valuable operator&(const Valuable& valuable) const { return Intersect(valuable); }
    Valuable& intersect(const Valuable& valuable, const Variable& va);
    Valuable& operator&=(const Valuable& valuable) { return intersect(valuable); }
    Valuable& intersect(const Valuable& valuable);
    Valuable Intersect(const Valuable& valuable, const Variable& va) const;

    Valuable Union(const Valuable& valuable) const;
    //Valuable operator|(const Valuable& valuable) const { return Union(valuable); }
    Valuable& unionize(const Valuable& valuable); // inplace
    //Valuable& unionize(const Valuable& valuable, const Variable& va);
    Valuable& operator|=(const Valuable& valuable) { return unionize(valuable); }
    //Valuable Union(const Valuable& valuable, const Variable& va) const;
    Valuable& remove(const Valuable& valuable);
    Valuable Remove(const Valuable& valuable) const;

    Valuable RootSetDifference(const Valuable& valuable) const;
    Valuable RootsSymetricDifference(const Valuable& valuable) const;

    /// conditional operators
    //
    Valuable Ifz(const Valuable& Then, const Valuable& Else) const; /// returns an expression which equals to @Then when this expression is zero and @Else otherwise
    Valuable IfNZ(const Valuable& Then, const Valuable& Else) const; /// returns an expression which equals to @Then when this expression is not zero and @Else if it is zero
    Valuable IfEq(const Valuable& v, const Valuable& Then,
                  const Valuable& Else) const; /// returns an expression which equals to @Then when this expression
                                               /// equals to @v param and @Else otherwise

    /// <summary>
    /// bool is 0 or 1
    /// </summary>
    /// <returns>0->1 or 1->0</returns>
    virtual Valuable BoolNot() const;

    /// <summary>
    /// 0 or 1
    /// </summary>
    /// <returns>0->0, otherwise 1</returns>
    virtual Valuable BoolIntModNotZero() const;

    /// <summary>
    /// IntMod prefix tells that this method is only applicable for variables known to be integer with modulo operation
    /// applicable
    /// x%(x-1) is 1 for x>2, is 0 for 2
    /// </summary>
    /// <returns>For x meant to be integers, returns an expression that is equal to 0 when x is positive</returns>
    virtual Valuable IntMod_IsPositive() const;

	/// <summary>
	/// (this < 0) - the int is negative
	/// </summary>
	/// <returns>bool</returns>
	virtual Valuable IntMod_Negative() const;

    /// <summary>
    /// Getting sign of the assumed integer
    /// </summary>
    /// <param name="a"></param>
    /// <returns>-1, 0, 1</returns>
    virtual Valuable IntMod_Sign() const;

	/// <summary>
	/// Converts the operator to boolean (delta function)
	/// </summary>
	/// <returns>An expression that evaluates to 1 or 0 value</returns>
	virtual Valuable ToBool() const;
    Valuable IfzToBool() const;

    /// <summary>
    /// (x-1)%x is -1 for negative numbers only
	/// (x-1)%x is undefined for zero
	/// (x-1)%x is x-1 for positive integers
    /// </summary>
    /// <returns></returns>
    virtual Valuable IntMod_IsNegativeOrZero() const;

    /// <summary>
    /// Operator 'less' then value to which a param expression is to be evaluated
	/// IntMod prefix tells that this method is only applicable for variables known to be integer
	/// Modulo operation may be used
    /// </summary>
    /// <param name="than">the param to compare that the object is less then the param</param>
    /// <returns>An expression that equals zero only when the object is less then param</returns>
    virtual Valuable IntMod_Less(const Valuable& than) const;

    /// <summary>
    /// (this <= 0) - constraint negative
    /// </summary>
    /// <returns>constraint values <= 0 : expression that equals zero for given negative or 0 *this values</returns>
    virtual Valuable NegativeOrZero() const;

    /// <summary>
    /// This > 0
    /// </summary>
    /// <returns>filter expression to have roots when this expression is positive</returns>
    virtual Valuable IsPositive() const;

    /// <summary>
    /// (this < 0) - constraint negative
    /// </summary>
    /// <returns>constraint to negative values: expression that equals zero for given negative *this values</returns>
    virtual Valuable IsNegative() const;

    /// <summary>
    /// This < 0, Than = abs(This)
    /// </summary>
    /// <param name="than"></param>
    /// <returns></returns>
    virtual Valuable IsNegativeThan(const Valuable& than) const;

    /// <summary>
    /// Operator 'less' then value to which a param expression is to be evaluated
    /// </summary>
    /// <param name="than">the param to compare that the object is less then the param</param>
    /// <returns>An expression that equals zero only when the object is less then param</returns>
	virtual Valuable Less(const Valuable& than) const;

    /// <summary>
    /// Operator 'less or eaual' then value to which a param expression is to be evaluated
    /// </summary>
    /// <param name="than">the param to compare the object with</param>
    /// <returns>An expression that equals zero only when the object is less or equal to param</returns>
    /// <remarks>returns -diff otherwise</remarks>
	virtual Valuable LessOrEqual(const Valuable& than) const;
	virtual Valuable GreaterOrEqual(const Valuable& than) const;

    /// <summary>
    /// Returns minimum between the object and the param asuming that both are real
    /// </summary>
    Valuable Minimum(const Valuable& second) const;

    /// <summary>
    /// Returns maximum between the object and the param asuming that both are real
    /// </summary>
    Valuable Maximum(const Valuable& with) const;

    /// <summary>
    /// Returns distance between the object and the param asuming that both are real
    /// </summary>
    Valuable Distance(const Valuable& with) const;

	/// iterations
	//
    Valuable For(const Valuable& initialValue, const Valuable& lambda) const; // TODO : iterations operator

	/// constrainting operators
	//
    virtual Valuable MustBeInt() const; /// return an expression which equals to zero only when this expression is integer (see https://math.stackexchange.com/a/1598552/118612)

	/// code builders
    //
    virtual std::function<bool(std::initializer_list<Valuable>)> Functor() const;

    size_t Hash() const;
    std::string str() const;
    std::wstring wstr() const;
    virtual std::wstring save(const std::wstring&) const;

    using variables_for_lambda_t = const std::initializer_list<const Variable>&; // passed recoursively, should be ref
    using universal_lambda_params_t = const std::initializer_list<const Valuable>&;
    using universal_lambda_t = std::function<Valuable(universal_lambda_params_t)>;
    virtual universal_lambda_t CompileIntoLambda(variables_for_lambda_t) const;

    template <typename... Fwd>
        requires((std::same_as<std::remove_cvref_t<Fwd>, ::omnn::math::Variable> ||
                  std::derived_from<std::remove_cvref_t<Fwd>, ::omnn::math::Variable>) &&
                 ...)
    [[nodiscard]]
    auto CompiLambda(Fwd&& ...variables) const {
        return [
            lambda = CompileIntoLambda(variables_for_lambda_t{std::forward<Fwd>(variables)...})
        ] (auto... args) {
            OptimizeOn on;
            return lambda({args...});
        };
    }

    virtual std::ostream& code(std::ostream& out) const;
    std::string OpenCL(const std::string_view& TypeName = "float") const;
    std::string OpenCLuint() const;
    va_names_t OpenCLparamVarNames() const;

    [[nodiscard]] virtual bool is_optimized() const;
    [[nodiscard]] Valuable Optimized() const;
    [[nodiscard]] Valuable Optimized(View) const;

protected:
    friend class boost::serialization::access;

    View view = View::Flat;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        if (exp) {
            ar & exp;
        } else {
            ar & hash;
            ar & maxVaExp;
            ar & view;
            ar & optimized;
        }
    }

};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Valuable)

template<class T>
const T& Valuable::as() const {
    auto& the = get();
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
    if (!the.Is<T>()) {
        return static_cast<const T&>(the);
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
