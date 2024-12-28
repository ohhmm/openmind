//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <list>
#include <unordered_set>
#include "ValuableCollectionDescendantContract.h"
#include "SumOrderComparator.h"
#include "Formula.h"

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
private:
    using base = ValuableCollectionDescendantContract<Sum, sum_cont>;
    using base::cont;
    friend class Variable;
    friend base;

    cont members;
    bool isOptimizing = false;
    mutable vars_cont_t vars;
    bool optimized = false;

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
    sum_cont& GetCont() override { return members; }
    const sum_cont& GetConstCont() const override { return members; }
    std::ostream& print(std::ostream& out) const override;
    
public:
    [[nodiscard]]
    Valuable* Clone() const override {
        return new Sum(*this);
    }

    void New(void* addr, Valuable&& v) override {
        new(addr) Sum(std::move(v.as<Sum>()));
    }

public:
    Valuable* Move() override {
        return new Sum(std::move(*this));
    }

    size_t getTypeSize() const override { return sizeof(Sum); }

    bool Same(const Valuable& v) const override {
        if (!v.Is<Sum>()) {
            return members.size() == 1 && members.begin()->Same(v);
        }
        const auto& other = v.as<Sum>();
        return members == other.members || (IsZero() && v.IsZero());
    }

    const iterator Add(const Valuable& item, iterator hint) override;
    const iterator Add(Valuable&& item, iterator hint) override;

public:
    static constexpr auto GetBinaryOperationLambdaTemplate() {
        return [](const auto& _1st, const auto& _2nd) { return _1st + _2nd; };
    }

    using base::base;
    using base::operator=;
    using base::Add;
    using iterator = typename sum_cont::iterator;
    using const_iterator = typename sum_cont::const_iterator;

    Sum() = default;
    Sum(Sum&&) = default;
    Sum(const Sum&) = default;
    Sum& operator=(Sum&&) = default;
    Sum& operator=(const Sum&) = default;
    explicit Sum(const Valuable& v);
    explicit Sum(Valuable&& v);
    Sum(const std::initializer_list<Valuable>& l);
    Sum(std::initializer_list<Valuable>&& l);

    void MarkAsOptimized() { optimized = true; }
    const Sum* CPtr() const { return this; }

    void Update(iterator& it, Valuable&& value) override;
    void Update(iterator& it, const Valuable& value) override;
    iterator Had(iterator it) override;

    bool IsPolynomial(const Variable& v) const override;
    bool SumIfSimplifiable(const Valuable& v) override;
    bool MultiplyIfSimplifiable(const Valuable& v) override;
    std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable& v) const override;
    std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;
    Valuable Sign() const override;

    static bool VarSurdFactor(const Valuable& v);
    bool operator==(const Sum& v) const;
    bool operator==(const Valuable& v) const override;
    Valuable operator-() const override;
    void optimize() override;

    Valuable GCDofMembers() const;
    Valuable GCDofCoefficients(const Variable& va) const;
    Valuable GCD(const Valuable& v) const override;
    bool IsZero() const override;

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
    Valuable LCMofMemberFractionDenominators() const override;
    Valuable SumOfRoots() const override;
    Valuable ProductOfRoots() const override;

    Valuable& operator+=(const Sum& sum);
    Valuable& operator+=(const Valuable& v) override;
    Valuable& operator*=(const Valuable& v) override;
    Valuable& operator/=(const Valuable& v) override;
    Valuable& operator%=(const Valuable& v) override;

    explicit operator double() const override;
    Valuable& d(const Variable& x) override;
    bool IsComesBefore(const Valuable& v) const override;

    std::ostream& code(std::ostream& out) const override;
    solutions_t Distinct() const override;
    bool IsBinomial() const;
    size_t FillPolynomialCoefficients(std::vector<Valuable>& coefficients, const Variable& v) const;
    Valuable operator()(const Variable& va, const Valuable& augmentation) const override;
    Valuable operator()(const Variable& va) const override;
    vars_cont_t GetVaExps() const override;

protected:
    bool IsPowerX(const std::vector<Valuable>& coefficients) const;

public:
    Formula FormulaOfVa(const Variable& v) const;
};

} // namespace omnn::math
