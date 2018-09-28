/*
 * Modulo.h
 *
 *  Created on: Sep 29, 2018
 *      Author: sergejkrivonos
 */

#pragma once

#include <omnn/math/ValuableDescendantContract.h>

namespace omnn
{
namespace math
{

class Modulo : public ValuableDescendantContract<Modulo>
{
    using base = ValuableDescendantContract<Modulo>;

    Valuable numerator, denominator;

public:
    Modulo(const Valuable& n, const Valuable& d);
    virtual ~Modulo();
};

} /* namespace math */
} /* namespace omnn */
