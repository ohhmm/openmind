#pragma once
#include <omnn/math/Variable.h>
#include <omnn/math/DuoValDescendant.h>

namespace omnn::math {

class Logarithm
    : public DuoValDescendant<Logarithm>
{
    using base = DuoValDescendant<Logarithm>;

protected:

 //   void InitVars() {
	//	commonVars = _1.GetVaExps();
	//	commonVars.insert(_2.GetVaExps().begin(), _2.GetVaExps().end());
	//}

	//mutable vars_cont_t commonVars;

	std::ostream& print(std::ostream&) const override;
	std::ostream& print_sign(std::ostream&) const override;

public:
	using base::base;
    using base::operator=;

	template <class BaseT, class TargetT>
    constexpr Logarithm(BaseT&& b, TargetT&& t)
    : base(std::forward<BaseT>(b), std::forward<TargetT>(t))
    {
        //InitVars();
    }

    Logarithm(const std::string_view&, std::shared_ptr<VarHost>, bool itIsOptimized);

    static constexpr auto GetBinaryOperationLambdaTemplate() {
        return [](const auto& base, const auto& target) {
            return ::std::log(target) / ::std::log(base);
        };
    }

    constexpr bool IsLogarithm() const override { return true; }
    void optimize() override;

    const Valuable& getBase() const { return _1; }
    const Valuable& lbase() const { return _1; }
    template <class T>
    void setBase(T&& b) {
        set1(::std::forward<T>(b));
        //InitVars();
        optimized = {};
    }
    template <class T>
    void updateBase(T&& b) {
        update1(std::forward<T>(b));
        //InitVars();
        optimized = {};
    }

    const Valuable& getTarget() const { return _2; }
    template <class T>
    void setTarget(T&& target) {
        set2(std::forward<T>(target));
        //InitVars();
        optimized = {};
    }
    template <class T>
    void updateTarget(T&& target) {
        update2(std::forward<T>(target));
        //InitVars();
        optimized = {};
    }

    static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2);

    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator *=(const Valuable& v) override;
    bool MultiplyIfSimplifiable(const Valuable& v) override;
    std::pair<bool,Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;
    bool SumIfSimplifiable(const Valuable& v) override;
    std::pair<bool,Valuable> IsSummationSimplifiable(const Valuable& v) const override;
    Valuable& operator /=(const Valuable&) override;
    Valuable& operator ^=(const Valuable&) override;
    explicit operator double() const override;
    Valuable& d(const Variable& x) override;
    Valuable& integral(const Variable& x, const Variable& C) override;
};

} // namespace omnn::math
