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
#include <sstream>
#include <string>
#include <typeindex>
#include <unordered_set>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/preprocessor.hpp>
#include <boost/rational.hpp>

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
class Exponentiation;
class Fraction;
class Sum;
size_t hash_value(const omnn::math::Valuable& v);
size_t hash_value(const omnn::math::Sum& v);
} // namespace math
} // namespace omnn

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

	template <>
    struct hash<omnn::math::Sum> {
        size_t operator()(const omnn::math::Sum& v) const { return hash_value(v); }
    };
}

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

class Valuable
        : public OpenOps<Valuable>
{
    using self = Valuable;


    static const a_int a_int_cz;
    static const max_exp_t max_exp_cz;

protected:
    using encapsulated_instance = ptrs::shared_ptr<Valuable>;
    encapsulated_instance exp = nullptr;
    const encapsulated_instance& getInst() const { return exp; }

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

    Valuable& Become(Valuable&& i);

    size_t hash = 0;
    size_t sz = sizeof(Valuable);
    static constexpr a_int const& a_int_z = a_int_cz;
    static constexpr max_exp_t const& max_exp_z = max_exp_cz;
    max_exp_t maxVaExp = 0;//max_exp_z; // ordering weight: vars max exponentiation in this valuable

public:

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
        Solving = 36, // Equation | 32
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

    enum class YesNoMaybe : uint8_t {
        Yes = 0b1, Maybe = 0b10, No = 0b100
    };

    const Valuable& get() const { return exp ? exp->get() : *this; }
    Valuable& get() { return exp ? exp->get() : *this; }

    template<class T>
    const T& as() const;

    template<class T>
    T& as() {
        auto& the = get();
        assert(the.Is<T>());
        return static_cast<T&>(the);
    }

    virtual encapsulated_instance SharedFromThis();

    //    friend constexpr operator bool(YesNoMaybe _) { return _==YesNoMaybe::Yes; }
    friend constexpr bool operator!(YesNoMaybe _) { return _ == YesNoMaybe::No; }
    friend constexpr YesNoMaybe operator||(YesNoMaybe _1, YesNoMaybe _2){
        constexpr omnn::math::Valuable::YesNoMaybe OrMap[] = {
            // Yes = 1, Maybe = 10, No = 100
            {},              // 000 bug
            omnn::math::Valuable::YesNoMaybe::Yes, // 001 yes
            omnn::math::Valuable::YesNoMaybe::Maybe, // 010 maybe
            omnn::math::Valuable::YesNoMaybe::Yes,   // 011  yes, maybe
            omnn::math::Valuable::YesNoMaybe::No, // 100 no
            omnn::math::Valuable::YesNoMaybe::Yes, // 101 yes,no
            omnn::math::Valuable::YesNoMaybe::Maybe, // 110 maybe,no
            omnn::math::Valuable::YesNoMaybe::Yes, // 111 yes,maybe,no
        };
        return OrMap[static_cast<uint8_t>(_1) | static_cast<uint8_t>(_2)];
    }
    friend constexpr YesNoMaybe operator&&(YesNoMaybe _1, YesNoMaybe _2){
        constexpr omnn::math::Valuable::YesNoMaybe AndMap[] = {
            // Yes = 1, Maybe = 10, No = 100
            {},              // 000 bug
            omnn::math::Valuable::YesNoMaybe::Yes, // 001 yes
            omnn::math::Valuable::YesNoMaybe::Maybe, // 010 maybe
            omnn::math::Valuable::YesNoMaybe::Maybe,   // 011  yes, maybe
            omnn::math::Valuable::YesNoMaybe::No, // 100 no
            omnn::math::Valuable::YesNoMaybe::No, // 101 yes,no
            omnn::math::Valuable::YesNoMaybe::No, // 110 maybe,no
            omnn::math::Valuable::YesNoMaybe::No, // 111 yes,maybe,no
        };
        return AndMap[static_cast<uint8_t>(_1) | static_cast<uint8_t>(_2)];
    }

    static thread_local bool optimizations;
    static thread_local bool bit_operation_optimizations;
    static thread_local bool enforce_solve_using_rational_root_test_only;

    class OptimizeOn {
        bool opts;
    public:
        OptimizeOn() : opts(optimizations) {
            optimizations = true;
        }
        ~OptimizeOn(){
            optimizations = opts;
        }
    };

    class OptimizeOff {
        bool opts;
    public:
        OptimizeOff() : opts(optimizations) {
			optimizations = {};
        }
        ~OptimizeOff(){
            optimizations = opts;
        }
    };

    explicit Valuable(Valuable* v);
    explicit Valuable(const encapsulated_instance& e);
    virtual std::type_index Type() const;
    const Valuable Link() const; // TODO : handle simulteneous values changes 

    Valuable& operator =(const Valuable& v);
    Valuable& operator =(Valuable&& v);

    bool SerializedStrEqual(const std::string_view& s) const;

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

    MSVC_CONSTEXPR Valuable(Valuable&&) = default;
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
    Valuable(const std::string_view&, std::shared_ptr<VarHost>, bool itIsOptimized = false);

    //constexpr
	virtual ~Valuable()//{}
        ;
    virtual Valuable operator -() const;
    virtual Valuable& operator +=(const Valuable&);
    virtual Valuable& operator +=(int);
    virtual Valuable& operator *=(const Valuable&);
    virtual bool MultiplyIfSimplifiable(const Valuable& v);
    virtual std::pair<bool,Valuable> IsMultiplicationSimplifiable(const Valuable& v) const;
    virtual bool SumIfSimplifiable(const Valuable& v);
    virtual std::pair<bool,Valuable> IsSummationSimplifiable(const Valuable& v) const;
    virtual Valuable& operator /=(const Valuable&);
    virtual Valuable& operator %=(const Valuable&);
    virtual Valuable& operator--();
    virtual Valuable& operator++();
    virtual Valuable& operator^=(const Valuable&);

    // returns the greatest common divisor (GCD) with the given object
    virtual Valuable GCD(const Valuable& v) const;
    virtual Valuable& gcd(const Valuable& v);

    // returns the least common multiple (LCM) with the given object
    virtual Valuable LCM(const Valuable& v) const;
    virtual Valuable& lcm(const Valuable& v);

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
    MSVC_CONSTEXPR APPLE_CONSTEXPR bool IsEquation() const {
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

    virtual YesNoMaybe IsEven() const;
    virtual YesNoMaybe IsMultival() const;
    virtual void Values(const std::function<bool(const Valuable&)>&) const; /// split multival to distinct values and return those using visitor pattern

    virtual bool is(const std::type_index&) const;

    template<class T>
    bool Is() const {
        return exp ? exp->Is<T>() : is(typeid(T));
    }

    virtual a_int Complexity() const;
    virtual Valuable& sq();
    virtual Valuable Sq() const;
    virtual Valuable Sign() const;
    virtual Valuable abs() const;
	virtual Valuable Cos() const;
	virtual Valuable Sin() const;
	virtual Valuable Sqrt() const;
    virtual Valuable& sqrt();
	virtual Valuable Tg() const;
    virtual void gamma(); // https://en.wikipedia.org/wiki/Gamma_function
    virtual Valuable Gamma() const;
    virtual Valuable& factorial();
    virtual Valuable Factorial() const;
    virtual Valuable calcFreeMember() const;

    virtual Valuable& reciprocal();
    virtual Valuable Reciprocal() const;

    using solutions_t = std::unordered_set<Valuable>;
    static Valuable MergeAnd(const Valuable&, const Valuable&); /// conjunctive merge of two values
    static Valuable MergeOr(const Valuable&, const Valuable&); /// disjunctive merge algorithm deduced from square equation solutions formula, works for integers
    static Valuable MergeOr(const Valuable&, const Valuable&, const Valuable&); /// algorithm is to be deduced from cubic equation solutions formula
    static Valuable MergeOr(const Valuable&, const Valuable&, const Valuable&, const Valuable&); /// double merge
    explicit Valuable(solutions_t&&);
    virtual Valuable operator()(const Variable&) const;
    virtual Valuable operator()(const Variable&, const Valuable& augmentation) const;
    bool IsUnivariable() const;

    virtual void solve(const Variable& va, solutions_t&) const;
    virtual solutions_t Distinct() const;
    virtual Valuable Univariate() const;
    virtual bool IsPolynomial(const Variable&) const;
    solutions_t Solutions() const;
    solutions_t IntSolutions() const;
    solutions_t Solutions(const Variable& v) const;
    solutions_t IntSolutions(const Variable& v) const;
    solutions_t GetIntegerSolution() const;
    virtual solutions_t GetIntegerSolution(const Variable& va) const;
    bool Test(const Variable& va, const Valuable& v) const;

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
    virtual Valuable InCommonWith(const Valuable& v) const;
    static const vars_cont_t& emptyCommonVars();
    virtual Valuable varless() const;
    static Valuable VaVal(const vars_cont_t& v);
    Valuable getVaVal() const;
    virtual bool eval(const vars_cont_t& with);
    Valuable Eval(const vars_cont_t& with) const;

    virtual const Variable* FindVa() const;
    virtual bool HasVa(const Variable&) const;
    using var_set_t = std::set<Variable>;
    virtual void CollectVa(var_set_t& s) const;
    using va_names_t = std::map<std::string_view, Variable>;
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
    virtual Valuable Or(const Valuable& n, const Valuable& v) const;
    virtual Valuable And(const Valuable& n, const Valuable& v) const;
    virtual Valuable Xor(const Valuable& n, const Valuable& v) const;
    virtual Valuable Not(const Valuable& n) const;
    virtual Valuable& shl();
    virtual Valuable& shl(const Valuable& n);
    virtual Valuable& shr(const Valuable& n);
    virtual Valuable& shr();
    virtual Valuable Shl(const Valuable& n) const;
    virtual Valuable Shr(const Valuable& n) const;
    virtual Valuable Shr() const;
    virtual Valuable sh(const Valuable& n) const;
    virtual Valuable Cyclic(const Valuable& total, const Valuable& shiftLeft) const;

    // logic
    static Valuable Abet(const Variable& x, std::initializer_list<Valuable>);
    template <class Fwd>
    constexpr Valuable& equals(Fwd&& v) {
//        SetView(View::Equation);  // FIXME: see ifz test
        return operator -=(std::forward<Fwd>(v));
    }
    Valuable Equals(const Valuable& v) const;
    Valuable NotEquals(const Valuable& v) const;
//    Valuable NE(const Valuable& to, const Valuable& abet) const; // not equals
//    Valuable NE(const Variable& x, const Valuable& to, std::initializer_list<Valuable> abet) const; // not equals
    Valuable LogicAnd(const Valuable& v) const;
    Valuable operator&&(const Valuable& v) const { return LogicAnd(v); }
    Valuable LogicOr(const Valuable& v) const;
    Valuable operator||(const Valuable& v) const { return LogicOr(v); }
    Valuable& logic_or(const Valuable&); // inplace
    Valuable& logic_and(const Valuable&);

    Valuable Intersect(const Valuable& with) const;
    Valuable operator&(const Valuable& v) const { return Intersect(v); }
    Valuable& intersect(const Valuable& with, const Variable& va);
    Valuable& operator&=(const Valuable& v) { return gcd(v); }
    Valuable Intersect(const Valuable& with, const Variable& va) const;

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
    virtual Valuable IntMod_IsNegativeOrZero() const { return Equals(0) || ((*this - 1) % *this).Equals(-1); }

    /// <summary>
    /// Operator 'less' then value to which a param expression is to be evaluated
	/// IntMod prefix tells that this method is only applicable for variables known to be integer
	/// Modulo operation may be used
    /// </summary>
    /// <param name="than">the param to compare that the object is less then the param</param>
    /// <returns>An expression that equals zero only when the object is less then param</returns>
    virtual Valuable IntMod_Less(const Valuable& than) const;

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

	virtual std::ostream& code(std::ostream& out) const;
    std::string OpenCL(const std::string_view& TypeName = "float") const;
    std::string OpenCLuint() const;
    va_names_t OpenCLparamVarNames() const;

    [[nodiscard]] virtual bool is_optimized() const;

protected:
    View view = View::Flat;
    bool optimized = false;
    void MarkAsOptimized();

    //   TODO : std::shared_ptr<std::vector<Valuable>> cachedValues;
};


Valuable implement(const char* str = "");

template<class T>
const T& Valuable::as() const {
    auto& the = get();
    if(!the.Is<T>()){
        IMPLEMENT
    }
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

template <typename ValueT>
class OptimizationLoopDetect {
    static thread_local std::unordered_set<ValueT> LoopDetectionStack;
    bool isLoop;
    const ValueT* value;

public:
    OptimizationLoopDetect(const ValueT& value) {
        auto emplaced = LoopDetectionStack.emplace(value);
        this->value = &*emplaced.first;
        isLoop = !emplaced.second;
    }

    ~OptimizationLoopDetect() {
        if (!isLoop)
            LoopDetectionStack.erase(*value);
    }

    auto isLoopDetected() const { return isLoop; }
};
template <typename ValueT>
thread_local std::unordered_set<ValueT> OptimizationLoopDetect<ValueT>::LoopDetectionStack;

} // namespace math
} // namespace omnn

::omnn::math::Valuable operator"" _v(const char* v, std::size_t);
const ::omnn::math::Variable& operator"" _va(const char* v, std::size_t);
//constexpr
::omnn::math::Valuable operator"" _v(unsigned long long v);
//constexpr const ::omnn::math::Valuable& operator"" _const(unsigned long long v);
::omnn::math::Valuable operator"" _v(long double v);


