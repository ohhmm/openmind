//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <list>
#include <unordered_set>
#include <boost/unordered_set.hpp>
#include "ValuableCollectionDescendantContract.h"
#include "Formula.h"

namespace omnn{
namespace extrapolator {

    using sum_cont =
        //std::multiset<Valuable, HashCompare>
        std::unordered_multiset<Valuable>
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

	Formula FormulaOfVa(const Variable& v) const;
};


}}
