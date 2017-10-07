//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <set>
#include "Fraction.h"
#include "Variable.h"

namespace omnn{
namespace extrapolator {

    struct ValuableLessCompare
    {
        bool operator()(const Valuable&, const Valuable&);
    };
    

class Product
    : public ValuableCollectionDescendantContract<Product, std::multiset<Valuable, ValuableLessCompare>>
{
    using base = ValuableCollectionDescendantContract<Product, std::multiset<Valuable, ValuableLessCompare>>;
    using base::cont;
    friend class Variable;
    cont members;
    std::multiset<Variable> vars;
    
protected:
    const cont& GetCont() const override { return members; }
	std::ostream& print(std::ostream& out) const override;

public:
    using base::base;

    void Add(const Valuable& item) override;
    
	template<class... T>
	Product(const T&... vals)
	{
		for (const auto& arg : { Valuable(vals)... })
		{
			Add(arg);
		}
	}

	Product() : members {{1}}
	{ }
    
    const std::multiset<Variable>& getCommonVars() const;
    Valuable varless() const;

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
};


}}
