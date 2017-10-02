//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <set>
#include "Fraction.h"
#include "Variable.h"

namespace omnn{
namespace extrapolator {

class Product
    : public ValuableCollectionDescendantContract<Product, std::set<Variable>>
{
    using base = ValuableCollectionDescendantContract<Product, std::set<Variable>>;
    using base::cont;
    friend class Variable;
    cont vars;
    
protected:
    const cont& GetCont() const override { return vars; }
    
public:
    using base::base;
    
    template<class... T>
    Product(const T&... vals)
    {
    }

};


}}
