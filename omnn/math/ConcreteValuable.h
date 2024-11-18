#pragma once

#include "ConcreteValuableForward.h"
#include "Valuable.h"
#include "VarHost.h"
#include <memory>
#include <string>
#include <string_view>
#include <typeindex>

namespace omnn::math {

class ConcreteValuable : public Valuable {
public:
    ConcreteValuable(const std::string& str, VarHost::ptr host, bool optimized = false);
    ConcreteValuable(const std::string& str, const Valuable::va_names_t& vaNames, bool optimized = false);
    ConcreteValuable(std::string_view str, const Valuable::va_names_t& vaNames, bool optimized = false);

    // Pure virtual interface implementations
    std::shared_ptr<Valuable> Clone() const noexcept override;
    std::shared_ptr<Valuable> Move() noexcept override;
    void New(void*, Valuable&&) noexcept override;
    size_t getTypeSize() const noexcept override;
    size_t getAllocSize() const noexcept override;
    void setAllocSize(size_t sz) noexcept override;
    bool IsSubObject(const Valuable& o) const noexcept override;
    bool HasVa(const Variable& va) const noexcept override;
    std::ostream& print(std::ostream& out) const noexcept override;
    std::wostream& print(std::wostream& out) const noexcept override;
    encapsulated_instance SharedFromThis() noexcept override;
    std::type_index Type() const noexcept override;
    ValuableWrapper Univariate() const noexcept override;
    ValuableWrapper InCommonWith(const Valuable& v) const noexcept override;

    // Virtual interface implementations
    bool IsVa() const noexcept override;
    void optimize() override;
    bool is_optimized() const noexcept override { return optimized_; }
    YesNoMaybe IsMultival() const noexcept override;
    bool IsSimple() const noexcept override;
    bool IsComesBefore(const Valuable& v) const noexcept override;
    a_int Complexity() const noexcept override;
    const Variable* FindVa() const noexcept override;
    void CollectVa(::std::set<Variable>& s) const noexcept override;
    void CollectVaNames(va_names_t& s) const noexcept override;
    bool eval(const ::std::map<Variable, ValuableWrapper>& with) noexcept override;
    void Eval(const Variable& va, const Valuable& v) noexcept override;
    void solve(const Variable& va, solutions_t& solutions) const noexcept override;
    const vars_cont_t& getCommonVars() const noexcept override;
    vars_cont_t GetVaExps() const noexcept override;

    // Operator overrides
    Valuable& operator=(Valuable&& v) noexcept override;
    Valuable& operator=(const Valuable& v) noexcept override;
    Valuable operator-() const noexcept override;
    Valuable& operator+=(const Valuable& v) noexcept override;
    Valuable& operator*=(const Valuable& v) noexcept override;
    bool MultiplyIfSimplifiable(const Valuable& v) noexcept override;
    bool SumIfSimplifiable(const Valuable& v) noexcept override;
    Valuable& operator/=(const Valuable& v) noexcept override;
    Valuable& operator%=(const Valuable& v) noexcept override;
    Valuable& operator^=(const Valuable& v) noexcept override;
    Valuable& d(const Variable& x) noexcept override;
    bool operator<(const Valuable& v) const noexcept override;
    bool operator==(const Valuable& v) const noexcept override;
    bool operator==(const Variable& v) const noexcept override;
    Valuable operator()(const Variable& va, const Valuable& augmentation) const override;

private:
    bool optimized_ = false;
    std::string expression_;
    VarHost::ptr host_;
    vars_cont_t common_vars_;
    size_t sz = sizeof(ConcreteValuable);  // Added sz member for allocation size
};

} // namespace omnn::math
