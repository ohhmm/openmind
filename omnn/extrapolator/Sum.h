//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <list>
#include <unordered_set>
#include "ValuableCollectionDescendantContract.h"
#include "Formula.h"

namespace omnn{
namespace extrapolator {


class Sum
        : public ValuableCollectionDescendantContract<Sum, std::unordered_multiset<Valuable>>
{
    using base = ValuableCollectionDescendantContract<Sum, std::unordered_multiset<Valuable>>;
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
    
    Sum() = default;
    
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
