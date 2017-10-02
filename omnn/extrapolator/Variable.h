//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <boost/any.hpp>
#include <memory>
#include "Valuable.h"
#include "VarHost.h"

namespace omnn{
namespace extrapolator {

    namespace any = boost;
    
class Variable
        : public ValuableDescendantContract<Variable>
{
    using base = ValuableDescendantContract<Variable>;
    VarHost::ptr optionalHostPtr;
    VarHost& varSetHost;
    any::any varId;
    
public:
    Variable(VarHost& varHost = const_cast<VarHost&>(VarHost::Global<>()))
    : varSetHost(varHost)
    , varId(varHost.NewVarId())
    { }
    
    Variable(VarHost::ptr varHost)
    : optionalHostPtr(varHost)
    , varSetHost(*varHost)
    , varId(varHost->NewVarId())
    {
        if(!varHost)
            throw "the varHost is mandatory parameter";
    }

    Valuable operator -() const override;
    Valuable& operator +=(const Valuable&) override;
    Valuable& operator +=(int) override;
    Valuable& operator *=(const Valuable&) override;
    Valuable& operator /=(const Valuable&) override;
//    Valuable& operator %=(const Valuable&) override;
    Valuable& operator--() override;
    Valuable& operator++() override;
    bool operator<(const Valuable& number) const override;
    bool operator==(const Valuable& number) const override;
    Valuable abs() const override { throw "Implement Abs Valuable Descendant to use it here"; }
//    void optimize() override;
//    Valuable sqrt() const override;
    std::ostream& print(std::ostream& out) const override;
    
    const Variable* FindVa() const override
    {
        return this;
    }
    
    using base::Become;
//    Valuable& Become(Valuable&& i) override
//    {
//        auto& b = base::Become(std::move(i));
//        //todo : notify other var instances through varhost
//        return b;
//    }
};


}}
