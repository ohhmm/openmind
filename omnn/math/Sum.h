//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <list>
#include <unordered_set>
#include "ValuableCollectionDescendantContract.h"
#include "Formula.h"
#include "SumOrderComparator.h"
//#include <boost/container/set.hpp>
//#include <boost/unordered_set.hpp>

namespace omnn::math {

    using sum_cont = // ensure Add/Update/Delete validness if you change this structure
        std::set<Valuable, SumOrderComparator>
//        boost::container::set<Valuable, SumOrderComparator>
//        std::unordered_multiset<Valuable>
//        boost::unordered_multiset<Valuable>
    ;

class Sum
    : public ValuableCollectionDescendantContract<Sum, sum_cont>
{
    using base = ValuableCollectionDescendantContract<Sum, sum_cont>;
    using base::cont;
    friend class Variable;
    cont members;
    bool isOptimizing = false;
    mutable vars_cont_t vars;

    class Optimizing {
        Sum &sum;
        bool opts;

    public:
        Optimizing(Sum &s)
        : sum(s)
        , opts(s.isOptimizing)
        {
            s.isOptimizing = true;
        }

        ~Optimizing() {
            if (sum.IsSum()) {
                sum.MarkAsOptimized();
                sum.isOptimizing = opts;
            }
        }
    };

protected:
	constexpr cont& GetCont() override { return members; }
	std::ostream& print(std::ostream& out) const override;
    
    [[nodiscard]]
    MSVC_CONSTEXPR
    Sum* Clone() const override
    {
        auto sum = new Sum(*CPtr());
        sum->isOptimizing = {};
        return sum;
    }

	template <typename T>
	const Sum::iterator SumAddImpl(T&& item, const iterator hint);

public:
    std::ostream& code(std::ostream& out) const override;

    static constexpr auto GetBinaryOperationLambdaTemplate() {
        return [](const auto& _1st, const auto& _2nd) { return _1st + _2nd; };
    }

	constexpr const cont& GetConstCont() const override { return members; }
    constexpr bool IsSum() const override { return true; }
    bool IsZero() const override;

	// virtual operators
	Valuable operator -() const override;
    Valuable& operator+=(const Sum& v);
	Valuable& operator +=(const Valuable& v) override;
	Valuable& operator *=(const Valuable& v) override;
	Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    explicit operator double() const override;
    Valuable& d(const Variable& x) override;
    Valuable GCDofMembers() const;
    Valuable GCDofCoefficients(const Variable&) const;
    Valuable LCMofMemberFractionDenominators() const;
    Valuable GCD(const Valuable& v) const override;

    bool operator ==(const Valuable& v) const override;

    void optimize() override;
    void balance();
    const PrincipalSurd* PrincipalSurdFactor() const override;
    void PerformSurdReduce();
    const vars_cont_t& getCommonVars() const override;
    Valuable InCommonWith(const Valuable& v) const override;
    max_exp_t findMaxVaExp();
    Valuable varless() const override;
    Valuable Sqrt() const override;
    Valuable& sq() override;
    Valuable calcFreeMember() const override;
    solutions_t GetIntegerSolution(const Variable& va) const override;
    void solve(const Variable&, solutions_t&) const override;
    void solve(const Variable&, solutions_t&, const std::vector<Valuable>& coefficients) const;
    void solve(const Variable&, solutions_t&, const std::vector<Valuable>& coefficients, size_t grade) const;
    Valuable SumOfRoots() const override;
    Valuable ProductOfRoots() const override;
    solutions_t Distinct() const override;

    static bool IsPowerX(const std::vector<Valuable>& coefficients);
    Valuable operator()(const Variable& va) const override;
    Valuable operator()(const Variable& va, const Valuable& augmentation) const override;
    
    using base::base;
    using base::operator =;
#if defined(_MSC_VER) || defined(__GNUC__)
    Sum() = default;
#endif
    Sum(Sum&&) = default;
    Sum(const Sum&) = default;
    Sum& operator=(Sum&&) = default;
    Sum& operator=(const Sum&) = default;
    bool operator==(const Sum& v) const;

    iterator Had(iterator it) override;
    static bool VarSurdFactor(const Valuable&);

    using base::Add;
    const iterator Add(Valuable&& item, const iterator hint) override;
    const iterator Add(const Valuable& item, const iterator hint) override;
    void Update(iterator&, Valuable&&) override;
    void Update(iterator&, const Valuable&) override;

    bool IsComesBefore(const Valuable& v) const override;

    Sum(std::initializer_list<Valuable> l);

    bool IsBinomial() const;
    bool IsPolynomial(const Variable&) const override;
    size_t FillPolynomialCoefficients(std::vector<Valuable>& coefficients, const Variable& v) const override;
    Formula FormulaOfVa(const Variable& v) const;

    bool SumIfSimplifiable(const Valuable& v) override;
    bool MultiplyIfSimplifiable(const Valuable& v) override;
    std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable& v) const override;
    std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;
	vars_cont_t GetVaExps() const override;
    Valuable Sign() const override;
};

} // namespace omnn::math
