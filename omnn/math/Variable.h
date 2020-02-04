//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
//#include <any>
#include <memory>
#include <boost/any.hpp>
#include "ValuableDescendantContract.h"

namespace omnn{
namespace math {

class VarHost;

class Variable
        : public ValuableDescendantContract<Variable>
{
    using base = ValuableDescendantContract<Variable>;
    std::shared_ptr<VarHost> varSetHost;
    boost::any varId;
    mutable vars_cont_t vars;
    
    friend class VarHost;
    void SetId(boost::any);

public:
    Variable();
    Variable(const Variable& v);
    Variable(std::shared_ptr<VarHost>);
    
    std::shared_ptr<VarHost> getVaHost() const override { return varSetHost; }

    Valuable operator -() const override;
    Valuable& operator +=(const Valuable&) override;
    Valuable& operator *=(const Valuable&) override;
    bool MultiplyIfSimplifiable(const Valuable& v) override;
    std::pair<bool,Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;
    bool SumIfSimplifiable(const Valuable& v) override;
    std::pair<bool,Valuable> IsSumationSimplifiable(const Valuable& v) const override;
    Valuable& operator /=(const Valuable&) override;
    Valuable& operator %=(const Valuable&) override;
    Valuable& operator ^=(const Valuable&) override;
    Valuable& d(const Variable& x) override;
    bool operator<(const Valuable& number) const override;
    bool operator==(const Valuable& number) const override;
    Valuable abs() const override { throw "Implement Abs Valuable Descendant to use it here"; }
    Valuable calcFreeMember() const override { return 0_v; }
//    void optimize() override;
//    Valuable sqrt() const override;
    std::ostream& print(std::ostream& out) const override;

    bool IsVa() const override { return true; }
    bool is_optimized() const override { return true; }
    YesNoMaybe IsMultival() const override { return YesNoMaybe::Maybe; }
    bool IsComesBefore(const Valuable& v) const override;
    a_int Complexity() const override { return 1; }
    
    const Variable* FindVa() const override { return this; }
    bool HasVa(const Variable& va) const override { return operator==(va); }
    
    void CollectVa(std::set<Variable>& s) const override;
    void CollectVaNames(std::map<std::string, Variable>& s) const override;

    bool eval(const std::map<Variable, Valuable>& with) override;
    
    void Eval(const Variable& va, const Valuable& v) override;
    
    const vars_cont_t& getCommonVars() const override;
    Valuable InCommonWith(const Valuable& v) const override;
    Valuable operator()(const Variable&, const Valuable& augmentation) const override;
};


}}

#define DECL_VA(x) auto& x = #x##_va;
