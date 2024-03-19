//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <any>
#include <memory>
//#include <boost/any.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include "ValuableDescendantContract.h"

namespace omnn{
namespace math {

namespace any = std;

class VarHost;

class Variable
        : public ValuableDescendantContract<Variable>
{
    using base = ValuableDescendantContract<Variable>;
    std::shared_ptr<VarHost> varSetHost;
    ::omnn::math::any::any varId;
    mutable vars_cont_t vars;
    
    friend class VarHost;
    void SetId(::omnn::math::any::any);
    const ::omnn::math::any::any& GetId() const;

protected:
    bool IsSubObject(const Valuable& o) const override {
        return this == &o.get();
    }

public:
    Variable();
    Variable(Variable&& v) = default;
    Variable& operator=(Variable&& v) = default;
    Variable& operator=(const Variable& v) = default;
    Variable(const Variable& v);
    Variable(std::shared_ptr<VarHost>);
    
    std::shared_ptr<VarHost> getVaHost() const override { return varSetHost; }
    const auto& getId() const { return varId; }

    Valuable operator -() const override;
    Valuable& operator +=(const Valuable&) override;
    Valuable& operator *=(const Valuable&) override;
    bool MultiplyIfSimplifiable(const Valuable& v) override;
    std::pair<bool,Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;
    bool SumIfSimplifiable(const Valuable& v) override;
    std::pair<bool,Valuable> IsSummationSimplifiable(const Valuable& v) const override;
    Valuable& operator /=(const Valuable&) override;
    Valuable& operator %=(const Valuable&) override;
    Valuable& operator ^=(const Valuable&) override;
    Valuable& d(const Variable& x) override;
    bool operator<(const Valuable&) const override;
    bool operator==(const Valuable&) const override;
    bool operator==(const Variable&) const;
    Valuable calcFreeMember() const override { return constants::zero; }
//    void optimize() override;
//    Valuable sqrt() const override;
    std::ostream& print(std::ostream& out) const override;
    std::ostream& code(std::ostream& out) const override { return print(out); }

    bool IsVa() const override { return true; }
    bool is_optimized() const override { return true; }
    YesNoMaybe IsMultival() const override { return YesNoMaybe::Maybe; }
    bool IsSimple() const override { return {}; }
    bool IsComesBefore(const Valuable& v) const override;
    a_int Complexity() const override { return 1; }
    
    const Variable* FindVa() const override { return this; }
    bool HasVa(const Variable& va) const override { return operator==(va); }
    
    void CollectVa(std::set<Variable>& s) const override;
    void CollectVaNames(Valuable::va_names_t& s) const override;

    bool eval(const std::map<Variable, Valuable>& with) override;
    void Eval(const Variable& va, const Valuable& v) override;
    void solve(const Variable& va, solutions_t& solutions) const override;
    
    const vars_cont_t& getCommonVars() const override;
    Valuable InCommonWith(const Valuable& v) const override;
    Valuable operator()(const Variable&, const Valuable& augmentation) const override;
    solutions_t Distinct() const override;

    bool IsPolynomial(const Variable&) const override;

    Valuable Abet(const std::initializer_list<Valuable>) const;

    vars_cont_t GetVaExps() const override;
};

}}

// Named variable with the same name text
#define DECL_VA(x) const ::omnn::math::Variable& x = #x##_va;
#define DECL_VA_MACRO(r, data, x) DECL_VA(x)
#define DECL_VARS(...) BOOST_PP_SEQ_FOR_EACH(DECL_VA_MACRO, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
