//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <list>
#include <unordered_set>
#include "ValuableCollectionDescendantContract.h"
#include "Formula.h"
//#include <boost/container/set.hpp>
//#include <boost/unordered_set.hpp>

namespace omnn{
namespace math {

    struct SumOrderComparator
    {
        bool operator()(const Valuable&, const Valuable&) const;
    };

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

protected:
	cont& GetCont() override { return members; }
	std::ostream& print(std::ostream& out) const override;
    
public:
	const cont& GetConstCont() const override { return members; }
    bool IsSum() const override { return true; }

	// virtual operators
	Valuable operator -() const override;
	Valuable& operator +=(const Valuable& v) override;
	Valuable& operator *=(const Valuable& v) override;
	Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    explicit operator double() const override;
    Valuable& d(const Variable& x) override;
    Valuable GCD() const;
    
    void optimize() override;
    const vars_cont_t& getCommonVars() const override;
    Valuable InCommonWith(const Valuable& v) const override;
    Valuable Sqrt() const override;
    Valuable& sq() override;
    Valuable calcFreeMember() const override;
    solutions_t GetIntegerSolution(const Variable& va) const override;
    void solve(const Variable& va, solutions_t& solutions) const override;
    void solve(const Variable& va, solutions_t& solutions, const std::vector<Valuable>& coefficients, size_t grade) const;

    Valuable operator()(const Variable& va) const override;
    Valuable operator()(const Variable& va, const Valuable& augmentation) const override;
    
    using base::base;
#if defined(_MSC_VER) || defined(__GNUC__)
    Sum() = default;
#endif
    
    iterator Had(iterator it) override;
    using base::Add;
    const iterator Add(const Valuable& item, const iterator hint) override;
    bool IsComesBefore(const Valuable& v) const override;

    Sum(const std::initializer_list<Valuable>& l);

    size_t FillPolyCoeff(std::vector<Valuable>& coefficients, const Variable& v) const;
	Formula FormulaOfVa(const Variable& v) const;

    bool SumIfSimplifiable(const Valuable& v) override;
    std::pair<bool,Valuable> IsSumationSimplifiable(const Valuable& v) const override;
};


}}
