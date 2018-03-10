//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <map>
#include <unordered_set>
#include "ValuableCollectionDescendantContract.h"
#include "Fraction.h"
#include "Variable.h"

namespace omnn{
namespace math {
    
    struct ProductOrderComparator
    {
        bool operator()(const Valuable&, const Valuable&) const;
    };
    
    using product_container =
        //std::multiset<Valuable, ProductOrderComparator>
        //std::unordered_multiset<Valuable>
        std::set<Valuable, ProductOrderComparator>
        ;

class Product
    : public ValuableCollectionDescendantContract<Product, product_container>
{
    using base = ValuableCollectionDescendantContract<Product, product_container>;
    using base::cont;
    friend class Variable;
    cont members;
    Integer vaExpsSum;

protected:
    void AddToVars(const Variable &item, const Valuable & exponentiation);
    void AddToVarsIfVaOrVaExp(const Valuable &item);

public:
    
    using base::base;

    Product();
    Product(const std::initializer_list<Valuable>& l);
    
	const cont& GetConstCont() const override { return members; }
    iterator Had(iterator it) override;
    const iterator Add(const Valuable& item) override;
    void Update(typename cont::iterator& it, const Valuable& v) override;
    void Delete(typename cont::iterator& it) override;

    const vars_cont_t& getCommonVars() const override;
    vars_cont_t getCommonVars(const vars_cont_t& with) const;
    
    Valuable getCommVal(const Product& with) const;
    int findMaxVaExp();
    bool IsComesBefore(const Valuable& v) const override;
    Valuable calcFreeMember() const override;
    
	// virtual operators
	Valuable& operator +=(const Valuable& v) override;
	Valuable& operator *=(const Valuable& v) override;
	Valuable& operator /=(const Valuable& v) override;
	Valuable& operator %=(const Valuable& v) override;
    Valuable& operator ^=(const Valuable& v) override;
	Valuable& operator --() override;
	Valuable& operator ++() override;
    Valuable& d(const Variable& x) override;
	void optimize() override;

	bool IsProduct() const override { return true; }
  
    solutions_t operator()(const Variable& va) const override;
    solutions_t operator()(const Variable&, const Valuable& augmentation) const override;
    
protected:
    cont& GetCont() override { return members; }
    std::ostream& print(std::ostream& out) const override;
    Product(const vars_cont_t& v) : vars(v) { Add(1_v); }
    
private:
    vars_cont_t vars;
};


}}
