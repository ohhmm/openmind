#pragma once
#include <omnn/math/Variable.h>
#include <omnn/math/DuoValDescendant.h>

namespace omnn::math {

/// This implementation objects kept if it is surd, otherwise it optimized out.
class PrincipalSurd /// Also known as principal root of Nth index
        : public DuoValDescendant<PrincipalSurd>
{
    using base = DuoValDescendant<PrincipalSurd>;

protected:
    Valuable& radicand() { return base::_1; }
    Valuable& index() { return base::_2; }

    std::ostream& print(std::ostream&) const override;
    std::ostream& print_sign(std::ostream&) const override;

public:
    using base::base;
    PrincipalSurd(const Valuable& radicand, const Valuable& index = 2);

    bool IsRadical() const override { return true; }
    bool IsPrincipalSurd() const override { return true; }
    constexpr const Valuable& Degree() const { return _2; }
    constexpr const Valuable& Radicand() const { return _1; }

    std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;

    void optimize() override;

    Valuable& sq() override;
    Valuable Sqrt() const override;

    Valuable Sign() const override;

    std::ostream& code(std::ostream&) const override;

    explicit operator double() const override;
    explicit operator long double() const override;
};

}
