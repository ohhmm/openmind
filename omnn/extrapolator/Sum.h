//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <list>
#include "Valuable.h"

namespace omnn{
namespace extrapolator {

class Sum
        : public ValuableDescentContract<Sum>
{
    std::list<Valuable> members;
public:
};


}}