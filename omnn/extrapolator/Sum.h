//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <list>
#include "Formula.h"

namespace omnn{
namespace extrapolator {


class Sum
        : public ValuableCollectionDescendantContract<Sum, std::list<Valuable>>
{
    using base = ValuableCollectionDescendantContract<Sum, std::list<Valuable>>;
    using base::cont;
    friend class Variable;
    cont members;
    bool isOptimizing = false;

protected:
    const cont& GetCont() const override { return members; }
	std::ostream& print(std::ostream& out) const override;
    
public:
    void Add(const Valuable& item) override;
    
	// virtual operators
	Valuable operator -() const override;
	Valuable& operator +=(const Valuable& v) override;
	Valuable& operator +=(int v) override;
	Valuable& operator *=(const Valuable& v) override;
	Valuable& operator /=(const Valuable& v) override;
	Valuable& operator %=(const Valuable& v) override;
	Valuable& operator --() override;
	Valuable& operator ++() override;
	bool operator <(const Valuable& v) const override;
	bool operator ==(const Valuable& v) const override;
    void optimize() override;
    Valuable sqrt() const override;

    using base::base;
    
    Sum() = default;
    
    Sum(const Valuable& f) {
        members.push_back(f);
    }
    
    template<class ...T>
    Sum(const T&... vals)
    {
		for (const auto& arg : { Valuable(vals)...})
		{
			members.push_back(arg);
		}
    }

	Formula FormulaOfVa(const Variable& v) const;
};


}}
