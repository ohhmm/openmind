#pragma once
#include <omnn/math/Variable.h>
#include <omnn/math/DuoValDescendant.h>

namespace omnn::math {

/// This implementation objects kept if it is surd, otherwise it optimized out.
class PrincipalSurd /// Also known as principal root of Nth index
        : public DuoValDescendant<PrincipalSurd>
{
    using base = DuoValDescendant<PrincipalSurd>;
    mutable vars_cont_t commonVars;

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
    constexpr const Valuable& Index() const { return _2; }
    constexpr const Valuable& Radicand() const { return _1; }
    template <class T>
    void setRadicand(T&& p) {
        set1(std::forward<T>(p));
    }

    std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable& v) const override;
    std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;

    void optimize() override;

    Valuable& sq() override;
    Valuable Sqrt() const override;

    Valuable Sign() const override;

    std::ostream& code(std::ostream&) const override;

    explicit operator double() const override;
    explicit operator long double() const override;

    vars_cont_t GetVaExps() const override;
    static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2);
	bool operator <(const Valuable&) const override;
    Valuable& operator^=(const Valuable&) override;
	bool IsComesBefore(const Valuable&) const override;
	Valuable InCommonWith(const Valuable&) const override;
    const Valuable::vars_cont_t& getCommonVars() const override;
};

}
