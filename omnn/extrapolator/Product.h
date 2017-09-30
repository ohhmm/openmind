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
        : public ValuableCollectionDescendantContract<Product>
{
    using base = ValuableCollectionDescendantContract<Product>;
    friend class Variable;
    std::set<Variable> vars;
    
public:
    using base::base;
    
    template<class... T>
    Product(const T&... vals)
    {
    }

    auto begin() const
    {
        return vars.begin();
    }

    auto end() const
    {
        return vars.end();
    }
    
    size_t size() const override
    {
        return vars.size();
    }
};


}}
