#pragma once
#include <omnn/math/Variable.h>
#include <omnn/math/DuoValDescendant.h>

namespace omnn::math {

class Limit
    : public DuoValDescendant<Limit>
{
    using base = DuoValDescendant<Limit>;
    Variable variable;

protected:
    std::ostream& print(std::ostream& out) const override;
    std::ostream& print_sign(std::ostream& out) const override;
    using base::set1;
    using base::set2;

public:
    using base::base;
    using base::operator=;
    using base::operator==;
    using base::update1;
    using base::update2;
    
    Limit(const std::string_view&, std::shared_ptr<VarHost>, bool itIsOptimized);
    
    template <class VarT, class ApproachT, class ExprT>
    constexpr Limit(const Variable& v, ApproachT&& a, ExprT&& e)
        : variable(v)
        , base(std::forward<ApproachT>(a), std::forward<ExprT>(e))
    {}

    static constexpr auto GetBinaryOperationLambdaTemplate() {
        return [](const auto& var, const auto& expr) {
            return expr;
        };
    }

    constexpr bool IsLimit() const { return true; }
    void optimize() override;

    constexpr const Variable& getLimitVar() const { return variable; }
    constexpr const Valuable& getApproachValue() const { return _1; }
    constexpr const Valuable& getExpression() const { return _2; }

    template <class T>
    void setLimitVar(T&& v) {
        variable = std::forward<T>(v);
        Valuable::hash = variable.Hash() ^ _1.Hash() ^ _2.Hash();
        optimized = {};
    }
    template <class T>
    void setApproachValue(T&& v) {
        set1(std::forward<T>(v));
        Valuable::hash = variable.Hash() ^ Valuable::hash;
    }
    template <class T>
    void setExpression(T&& v) {
        set2(std::forward<T>(v));
        Valuable::hash = variable.Hash() ^ Valuable::hash;
    }

    static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2);
    vars_cont_t GetVaExps() const override;

    bool operator==(const Valuable& other) const override;
    explicit operator double() const override;
    Valuable& d(const Variable& x) override;
    Valuable& integral(const Variable& x, const Variable& C) override;
};

} // namespace omnn::math
