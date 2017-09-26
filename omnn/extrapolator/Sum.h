//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <list>
#include "Fraction.h"
#include "Variable.h"

namespace omnn{
namespace extrapolator {

class Sum
        : public ValuableDescendantContract<Sum>
{
    using base = ValuableDescendantContract<Sum>;
    friend class Variable;
    std::list<Valuable> members;
public:
    using base::base;
    
    Sum() = default;
    
    Sum(const Valuable& f) {
        members.push_back(f);
    }
    
    template<class ...T>
    Sum(const T&... vals)
    {
    }
};


}}
