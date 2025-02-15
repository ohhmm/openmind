#pragma once
#include <omnn/math/Constant.h>

namespace omnn::math {

class NaN : public Constant<NaN> {
    using base = Constant<NaN>;
    Valuable reason;

public:
    using base::base;

    NaN(encapsulated_instance&& reason);

    static /*constinit*/ std::string_view SerializationName;

    constexpr
    bool IsNaN() const override { return true; }

    bool IsSimple() const noexcept override { return {}; }
    Valuable ToBool() const override { return {}; }

    Valuable* Clone() const noexcept override {
        auto inst = reason.getInst();
        return new NaN(std::move(inst));
    }

    Valuable operator-() const override { return *this; }
    Valuable& operator+=(const Valuable& v) override { return *this; }
    Valuable& operator*=(const Valuable& v) noexcept override { return *this; }
    Valuable& operator/=(const Valuable& v) override { return *this; }
    Valuable& operator%=(const Valuable& v) override { return *this; }
    constexpr Valuable& operator--() override { return *this; }
    constexpr Valuable& operator++() override { return *this; }
    constexpr Valuable& operator^=(const Valuable&) override { return *this; }
    constexpr Valuable& d(const Variable& x) override { return *this; }
    constexpr Valuable& sqrt() override { return *this; }
    Valuable Sqrt() const override { return *this; }
    constexpr bool IsComesBefore(const Valuable& v) const override { return !v.IsNaN(); }  // NaN comes before non-NaN values, but not before other NaNs

    std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable&) const override { return {true, *this}; }
};

namespace constant {
static const NaN nan;
}

} // namespace omnn::math
