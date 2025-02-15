#pragma once
#include <omnn/math/Constant.h>

namespace omnn{
namespace math {

class Infinity;
class MInfinity;

class Infinity
        : public Constant<Infinity>
{
    using base = Constant<Infinity>;

public:
    using base::base;

    static /*constinit*/ std::string_view SerializationName;

    bool IsInfinity() const override { return true; }

    bool IsSimple() const noexcept override { return {}; }

    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator *=(const Valuable& v) noexcept override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override { return *this; }
    Valuable& operator ++() override { return *this; }
    Valuable& operator^=(const Valuable&) override;
    Valuable& d(const Variable& x) override;
    bool operator <(const Valuable& v) const override;

    std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable&) const override;
    std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable&) const override;
};

class MInfinity
        : public Constant<MInfinity>
{
    using base = Constant<MInfinity>;

public:
    using base::base;

    static /*constinit*/ std::string_view SerializationName;

    bool IsMInfinity() const override { return true; }

    bool IsSimple() const noexcept override { return {}; }

    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator *=(const Valuable& v) noexcept override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override { return *this; }
    Valuable& operator ++() override { return *this; }
    Valuable& operator^=(const Valuable&) override;
    Valuable& d(const Variable& x) override;
    bool operator <(const Valuable& v) const override;
    
    bool IsComesBefore(const Valuable& v) const override { return true; }
    std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable&) const override;
};

}}
