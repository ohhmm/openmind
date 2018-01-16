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
namespace math {

    struct SumOrderComparator
    {
        bool operator()(const Valuable&, const Valuable&) const;
    };

    using sum_cont = // ensure Add/Update/Deletehas right behaviour if you change this structure
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
    bool IsSum() const override { return true; }

	// virtual operators
	Valuable operator -() const override;
	Valuable& operator +=(const Valuable& v) override;
	Valuable& operator *=(const Valuable& v) override;
	Valuable& operator /=(const Valuable& v) override;
	Valuable& operator --() override;
	Valuable& operator ++() override;
    void optimize() override;
    Valuable sqrt() const override;
    Valuable calcFreeMember() const override;

    using base::base;
    
    const iterator Add(const Valuable& item) override;
    void Update(typename cont::iterator& it, const Valuable& v) override;
    bool IsComesBefore(const Valuable& v) const override;

    Sum(const Valuable& f) {
        Add(f);
    }
    
    template<class ...T>
    Sum(const T&... vals)
    {
		for (const auto& arg : { Valuable(vals)...})
		{
            auto a = cast(arg);
            if(a)
                for(auto& m: *a)
                    Add(m);
            else
                Add(arg);
		}
    }

    size_t FillPolyCoeff(std::vector<Valuable>& coefficients, const Variable& v) const;
	Formula FormulaOfVa(const Variable& v) const;
};


}}
