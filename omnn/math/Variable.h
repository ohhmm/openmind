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

    Valuable operator -() const override;
    Valuable& operator +=(const Valuable&) override;
    Valuable& operator *=(const Valuable&) override;
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
    bool IsComesBefore(const Valuable& v) const override;
    
    const Variable* FindVa() const override { return this; }
    bool HasVa(const Variable& va) const override { return operator==(va); }
    
    void CollectVa(std::set<Variable>& s) const override;

    Valuable& eval(const std::map<Variable, Valuable>& with) override;
    
    void Eval(const Variable& va, const Valuable& v) override;
    
    const vars_cont_t& getCommonVars() const override;
    Valuable operator()(const Variable&, const Valuable& augmentation) const override;
};


}}
