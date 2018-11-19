/*
 * Modulo.h
 *
 *  Created on: Sep 29, 2018
 *      Author: sergejkrivonos
 */

#pragma once

#include <omnn/math/DuoValDescendant.h>

namespace omnn
{
namespace math
{

class Modulo : public DuoValDescendant<Modulo>
{
    using base = DuoValDescendant<Modulo>;

public:
    using base::base;

    std::ostream& print_sign(std::ostream& out) const override;
    
    static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2) {
        return _1.getMaxVaExp();
    }
    
    void optimize() override;
};

} /* namespace math */
} /* namespace omnn */
