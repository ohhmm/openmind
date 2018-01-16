//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <boost/any.hpp>
#include <memory>
#include "ValuableDescendantContract.h"
#include "VarHost.h"

namespace omnn{
namespace math {

    namespace any = boost;
    
class Variable
        : public ValuableDescendantContract<Variable>
{
    using base = ValuableDescendantContract<Variable>;
    VarHost::ptr optionalHostPtr;
    VarHost* varSetHost;
    any::any varId;
    mutable vars_cont_t vars;
    
public:
    Variable(VarHost& varHost = const_cast<VarHost&>(VarHost::Global<>()))
    : varSetHost(&varHost)
    , varId(varHost.NewVarId())
    {
        hash = varHost.Hash(varId);
        maxVaExp=1;
    }
    
    Variable(VarHost::ptr varHost)
    : optionalHostPtr(varHost)
    , varSetHost(varHost.get())
    , varId(varHost->NewVarId())
    {
        if(!varHost)
            throw "the varHost is mandatory parameter";
        hash = varHost->Hash(varId);
        maxVaExp=1;
    }

    Valuable operator -() const override;
    Valuable& operator +=(const Valuable&) override;
    Valuable& operator +=(int) override;
    Valuable& operator *=(const Valuable&) override;
    Valuable& operator /=(const Valuable&) override;
//    Valuable& operator %=(const Valuable&) override;
    Valuable& operator--() override;
    Valuable& operator++() override;
    Valuable& operator ^=(const Valuable&) override;
    bool operator<(const Valuable& number) const override;
    bool operator==(const Valuable& number) const override;
    Valuable abs() const override { throw "Implement Abs Valuable Descendant to use it here"; }
    Valuable calcFreeMember() const override { return 0_v; }
//    void optimize() override;
//    Valuable sqrt() const override;
    std::ostream& print(std::ostream& out) const override;

    bool IsVa() const override { return true; }
    bool IsComesBefore(const Valuable& v) const override;
    
    const Variable* FindVa() const override
    {
        return this;
    }
    
    void CollectVa(std::set<Variable>& s) const override;
    
    void Eval(const Variable& va, const Valuable& v) override;
    
    const vars_cont_t& getCommonVars() const override;
};


}}
