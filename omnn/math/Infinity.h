#pragma once
#include "ValuableDescendantContract.h"

namespace omnn{
namespace math {

class Infinity;
class MInfinity;

class Infinity
        : public ValuableDescendantContract<Infinity>
{
    using base = ValuableDescendantContract<Infinity>;

protected:
    std::ostream& print(std::ostream& out) const override;

public:
    using base::base;

    bool IsInfinity() const override { return true; }
    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override { return *this; }
    Valuable& operator ++() override { return *this; }
    Valuable& operator^=(const Valuable&) override;
    Valuable& d(const Variable& x) override;
    bool operator <(const Valuable& v) const override;
};

class MInfinity
        : public ValuableDescendantContract<MInfinity>
{
    using base = ValuableDescendantContract<MInfinity>;

protected:
    std::ostream& print(std::ostream& out) const override;

public:
    using base::base;

    bool IsMInfinity() const override { return true; }
    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override { return *this; }
    Valuable& operator ++() override { return *this; }
    Valuable& operator^=(const Valuable&) override;
    Valuable& d(const Variable& x) override;
    bool operator <(const Valuable& v) const override;
};

}}
