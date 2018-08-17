//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "ValuableDescendant.h"

namespace omnn::math {

    class Euler : public ValuableDescendant<Euler>
    {
        using base = ValuableDescendant<Euler>;

    protected:
        std::ostream &print(std::ostream &out) const override
        { return out << 'e'; }

    public:
        using base::base;

        bool Is_e() const override { return true; }

//        bool operator<(const Valuable &v) const override;
    };

    const Euler e;
}
