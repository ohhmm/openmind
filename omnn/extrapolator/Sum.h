//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <list>
#include "Fraction.h"

namespace omnn{
namespace extrapolator {

class Sum
        : public ValuableDescendantContract<Sum>
{
    using base = ValuableDescendantContract<Sum>;
    std::list<Valuable> members;
public:
    using base::base;
    Sum() = default;
    Sum(const Fraction& f) {
        members.push_back(f);
    }
};


}}