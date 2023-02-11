/*
 * Modulo.h
 *
 *  Created on: Sep 29, 2018
 *      Author: sergejkrivonos
 */

#pragma once

#include <omnn/math/DuoValDescendant.h>

namespace omnn::math
{

class Modulo : public DuoValDescendant<Modulo>
{
    using base = DuoValDescendant<Modulo>;
    static vars_cont_t VarsForCommoning;

public:
    using base::base;

	bool IsModulo() const override { return true; }

    std::ostream& print_sign(std::ostream& out) const override;
    std::ostream& code_sign(std::ostream& out) const override;
    std::ostream& code(std::ostream& out) const override;
    
    static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2);
    
    void optimize() override;

	Valuable operator-() const override;

	std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable& v) const override { return {}; }
    std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override { return {}; }
    Valuable InCommonWith(const Valuable& v) const override { return 1; }
    const vars_cont_t& getCommonVars() const override { return VarsForCommoning; }

	bool IsComesBefore(const Valuable& v) const override;
};

} /* namespace omnn::math */
