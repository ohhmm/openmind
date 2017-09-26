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
    std::shared_ptr<VarHost> varSetHost;
    any::any varId;
    Variable() = delete;
public:
    Variable(std::shared_ptr<VarHost> varHost)
    : varSetHost(varHost)
    , varId(varHost->NewVarId())
    { }

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
    Valuable sqrt() const override;
    std::ostream& print(std::ostream& out) const override;
};


}}
