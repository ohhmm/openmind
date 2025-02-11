#pragma once
#include <omnn/math/Variable.h>
#include <omnn/math/DuoValDescendant.h>
#include <omnn/math/Fraction.h>

namespace omnn::math {

class Limit
    : public DuoValDescendant<Limit>
{
    using base = DuoValDescendant<Limit>;
    Variable limitVar;
    Valuable approachValue;

protected:
    std::ostream& print(std::ostream& out) const override;
    std::ostream& print_sign(std::ostream& out) const override;

public:
    using base::base;
    using base::operator=;
    using base::operator==;

    template <class VarT, class ExprT>
    constexpr Limit(VarT&& v, ExprT&& e)
    : base(std::forward<VarT>(v), std::forward<ExprT>(e))
    {
    }

    Limit(const std::string_view&, std::shared_ptr<VarHost>, bool itIsOptimized);

    static constexpr auto GetBinaryOperationLambdaTemplate() {
        return [](const auto& var, const auto& expr) {
            return expr;
        };
    }

    constexpr bool IsLimit() const override { return true; }
    void optimize() override;

    const Variable& getLimitVar() const { return limitVar; }
    const Valuable& getApproachValue() const { return approachValue; }
    template <class T>
    void setLimitVar(T&& v) {
        limitVar = std::forward<T>(v);
        optimized = {};
    }
    template <class T>
    void setApproachValue(T&& v) {
        approachValue = std::forward<T>(v);
        optimized = {};
    }

    static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2);
    vars_cont_t GetVaExps() const override;

    bool operator==(const Valuable& other) const override;
    explicit operator double() const override;
    Valuable& d(const Variable& x) override;
    Valuable& integral(const Variable& x, const Variable& C) override;
};

} // namespace omnn::math
