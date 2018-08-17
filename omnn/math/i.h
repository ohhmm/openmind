//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "ValuableDescendant.h"

namespace omnn::math {

    class MinusOneSq : public ValuableDescendant<MinusOneSq>
    {
        using base = ValuableDescendant<MinusOneSq>;

    protected:
        std::ostream &print(std::ostream &out) const override
        { return out << 'i'; }

    public:
        using base::base;

        bool Is_i() const override { return true; }
    };

    const MinusOneSq i;
}
