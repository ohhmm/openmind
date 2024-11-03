//
// Created by Сергей Кривонос on 01.09.17.
#pragma once

#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <map>
#include <set>
#include <deque>
#include <functional>
#include <list>
#include <sstream>
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

#include "ValuableWrapper.h"
#include "OpenOps.h"

#ifdef TBB_PREVIEW_PARTITIONER_1_0
namespace tbb {
namespace detail {
namespace d1 {
    class static_partition_type {
    public:
        size_t my_divisor;
        size_t do_split(const static_partition_type& src, const proportional_split& split_obj) {
            return split_obj.right();
        }
    };

    class affinity_partition_type {
    public:
        size_t my_divisor;
        size_t do_split(const affinity_partition_type& src, const proportional_split& split_obj) {
            return split_obj.right();
        }
    };
}}}
#endif

#define _NUM2STR(x) #x
#define NUM2STR(x) _NUM2STR(x)
#define LINE_NUMBER_STR NUM2STR(__LINE__)
#define IMPLEMENT = 0;
#define LOG_AND_IMPLEMENT(Param) = 0;

namespace omnn {
namespace math {

class Valuable;  // Forward declaration

// Function declarations
Valuable abs(const Valuable& v);
Valuable log(const Valuable&);
Valuable pow(const Valuable&, const Valuable&);
Valuable sqrt(const Valuable& v);
Valuable tanh(const Valuable&);

namespace constants {
    // Constants namespace
}

using a_int = boost::multiprecision::cpp_int;
using a_rational = boost::multiprecision::cpp_rational;
using max_exp_t = a_rational;
namespace ptrs = ::std;

class VarHost;
struct ValuableDescendantMarker {};

class Valuable : public std::enable_shared_from_this<Valuable>, public OpenOps<Valuable> {
public:
    // Move operations
    virtual ~Valuable() noexcept = 0;

public:
    using ptr = std::shared_ptr<Valuable>;
    using const_ptr = std::shared_ptr<const Valuable>;
    using self = Valuable;
    using encapsulated_instance = std::shared_ptr<Valuable>;
    using vars_cont_t = std::set<std::shared_ptr<Variable>>;
    using va_names_t = std::set<std::string>;
    using universal_lambda_t = std::function<Valuable(const std::vector<Valuable>&)>;
    using universal_lambda_params_t = const std::vector<Valuable>&;
    using variables_for_lambda_t = std::vector<Variable>;
    using solutions_t = std::vector<ValuableWrapper>;

    // Special member functions
    Valuable() noexcept;
    Valuable(const Valuable&) = delete;
    Valuable& operator=(const Valuable&) = delete;
    Valuable(Valuable&&) noexcept = default;
    Valuable& operator=(Valuable&&) noexcept = default;
    virtual ~Valuable() noexcept = 0;

    // Pure virtual interface
    virtual std::shared_ptr<Valuable> Clone() const noexcept = 0;
    virtual std::shared_ptr<Valuable> Move() noexcept = 0;
    virtual void New(void*, Valuable&&) noexcept = 0;
    virtual size_t getTypeSize() const noexcept = 0;
    virtual size_t getAllocSize() const noexcept = 0;
    virtual void setAllocSize(size_t sz) noexcept = 0;
    virtual bool IsSubObject(const Valuable& o) const noexcept = 0;
    virtual bool HasVa(const Variable& va) const noexcept = 0;
    virtual std::ostream& print(std::ostream& out) const noexcept = 0;
    virtual std::wostream& print(std::wostream& out) const noexcept = 0;
    virtual encapsulated_instance SharedFromThis() noexcept = 0;
    virtual std::type_index Type() const noexcept = 0;
    virtual ValuableWrapper Univariate() const noexcept = 0;
    virtual ValuableWrapper InCommonWith(const Valuable& v) const noexcept = 0;

    // Constants
    static const a_int a_int_cz;
    static const max_exp_t max_exp_cz;
    static constexpr a_int const& a_int_z = a_int_cz;
    static constexpr max_exp_t const& max_exp_z = max_exp_cz;
    static constexpr size_t DefaultAllocSize = 768;

    // View enumeration
    enum class View : uint8_t {
        Default,
        Equation,
        Flat
    };

    enum class YesNoMaybe : uint8_t {
        No,
        Yes,
        Maybe
    };

    // Public interface
    const Valuable& get() const & noexcept { return *this; }
    Valuable& get() & noexcept { return *this; }
    Valuable get() && noexcept { return std::move(*this); }

    template<typename T>
    const T& as() const {
        return dynamic_cast<const T&>(*this);
    }

    template<typename T>
    T& as() noexcept {
        return dynamic_cast<T&>(*this);
    }

    template<typename T>
    T* As() noexcept {
        return dynamic_cast<T*>(this);
    }

    // Public access to maxVaExp
    const max_exp_t& getMaxVaExp() const noexcept { return maxVaExp; }


    // Virtual interface
    virtual bool IsVa() const noexcept = 0;
    virtual bool is_optimized() const noexcept = 0;
    virtual YesNoMaybe IsMultival() const noexcept = 0;
    virtual bool IsSimple() const noexcept = 0;
    virtual bool IsComesBefore(const Valuable& v) const noexcept = 0;
    virtual a_int Complexity() const noexcept = 0;
    virtual const Variable* FindVa() const noexcept = 0;
    virtual bool HasVa(const Variable& va) const noexcept = 0;
    virtual void CollectVa(::std::set<Variable>& s) const noexcept = 0;
    virtual void CollectVaNames(va_names_t& s) const noexcept = 0;
    virtual bool eval(const ::std::map<Variable, ValuableWrapper>& with) noexcept = 0;
    virtual void Eval(const Variable& va, const Valuable& v) noexcept = 0;
    virtual void solve(const Variable& va, solutions_t& solutions) const noexcept = 0;
    virtual const vars_cont_t& getCommonVars() const noexcept = 0;
    virtual vars_cont_t GetVaExps() const noexcept = 0;
    virtual Valuable& operator=(Valuable&& v) noexcept = 0;
    virtual Valuable& operator=(const Valuable& v) noexcept = 0;
    virtual Valuable operator-() const noexcept = 0;
    virtual Valuable& operator+=(const Valuable& v) noexcept = 0;
    virtual Valuable& operator*=(const Valuable& v) noexcept = 0;
    virtual bool MultiplyIfSimplifiable(const Valuable& v) noexcept = 0;
    virtual bool SumIfSimplifiable(const Valuable& v) noexcept = 0;
    virtual Valuable& operator/=(const Valuable& v) noexcept = 0;
    virtual Valuable& operator%=(const Valuable& v) noexcept = 0;
    virtual Valuable& operator^=(const Valuable& v) noexcept = 0;
    virtual Valuable& d(const Variable& x) noexcept = 0;
    virtual bool operator<(const Valuable& v) const noexcept = 0;
    virtual bool operator==(const Valuable& v) const noexcept = 0;
    virtual bool operator==(const Variable& v) const noexcept = 0;
    virtual Valuable operator()(const Variable& va, const Valuable& augmentation) const = 0;

protected:
    // Protected members
    encapsulated_instance exp;
    size_t hash = 0;
    size_t sz = sizeof(Valuable);
    max_exp_t maxVaExp = 0;
    bool optimized = false;

    // Protected interface
    void MarkAsOptimized() { optimized = true; }
    Valuable& Become(Valuable&& i);

    // Protected constructors
    Valuable(ValuableDescendantMarker) noexcept {}
    Valuable(const Valuable& v, ValuableDescendantMarker) {}
    Valuable(Valuable&& v, ValuableDescendantMarker) noexcept {}
};

// Friend functions
std::ostream& operator<<(std::ostream& os, Valuable::View v) noexcept;
constexpr Valuable::View operator&(Valuable::View a, Valuable::View b) noexcept;
constexpr Valuable::View operator|(Valuable::View a, Valuable::View b) noexcept;
constexpr bool operator!(Valuable::YesNoMaybe _) noexcept;

// Literal operators
Valuable operator"" _v(unsigned long long v);
Valuable operator"" _v(long double v);

} // namespace math

// Hash specialization
template<>
struct std::hash<math::Valuable> {
    size_t operator()(const math::Valuable& v) const noexcept;
};

} // namespace omnn
