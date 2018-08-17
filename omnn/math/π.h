//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "ValuableDescendant.h"

namespace omnn::math {

    class Pi : public ValuableDescendant<Pi>
    {
        using base = ValuableDescendant<Pi>;

    protected:
        std::ostream &print(std::ostream &out) const override
        { return out << "pi"; }

    public:
        using base::base;

        bool Is_π() const override { return true; }
    };

    const Pi π;
}
