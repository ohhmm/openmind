//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma pragma once
#include "ValuableDescendant.h"

namespace omnn::math {

    class e : public ValuableDescendant<e>
    {
        using base = ValuableDescendant<e>;

    protected:
        std::ostream &print(std::ostream &out) const override;

    public:
        using base::base;

        bool Is_e() const override { return true; }
//
//        Valuable &d(const Variable &x) override;
//
//        bool operator<(const Valuable &v) const override;
    };

}
