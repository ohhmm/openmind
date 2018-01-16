#pragma once

#include "ValuableDescendantContract.h"

namespace omnn{
namespace math {

/**
 * for accessing a valuable members
 */
class Accessor
        : public ValuableDescendantContract<Accessor>
{
    friend class Valuable;
    Valuable& v;
    size_t& h;
public:
    Accessor(Valuable& m, size_t& hash);

    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override;
    Valuable& operator ++() override;
    Valuable& operator^=(const Valuable&) override;
    bool operator <(const Valuable& v) const override;
    bool operator ==(const Valuable& v) const override;

    const Variable* FindVa() const override;
    void CollectVa(std::set<Variable>&) const override;
    void Eval(const Variable& va, const Valuable& v) override;
};

}}
