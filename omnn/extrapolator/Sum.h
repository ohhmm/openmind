//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <list>
#include <unordered_set>
#include "ValuableCollectionDescendantContract.h"
#include "Formula.h"
#include <boost/container/set.hpp>
#include <boost/unordered_set.hpp>

namespace omnn{
namespace extrapolator {

    struct SumOrderComparator
    {
        bool operator()(const Valuable&, const Valuable&) const;
    };

    using sum_cont =
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

protected:
	cont& GetCont() override { return members; }
	std::ostream& print(std::ostream& out) const override;
    
public:
	const cont& GetConstCont() const override { return members; }
    
	// virtual operators
	Valuable operator -() const override;
	Valuable& operator +=(const Valuable& v) override;
	Valuable& operator *=(const Valuable& v) override;
	Valuable& operator /=(const Valuable& v) override;
	Valuable& operator --() override;
	Valuable& operator ++() override;
    void optimize() override;
    Valuable sqrt() const override;

    using base::base;
    
    void Add(const Valuable& item) override;
    
    Sum(const Valuable& f) {
        Add(f);
    }
    
    template<class ...T>
    Sum(const T&... vals)
    {
		for (const auto& arg : { Valuable(vals)...})
		{
			Add(arg);
		}
    }

    size_t FillPolyCoeff(std::vector<Valuable>& coefficients, const Variable& v) const;
	Formula FormulaOfVa(const Variable& v) const;
};


}}
