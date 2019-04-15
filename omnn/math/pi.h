//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "Constant.h"

namespace omnn::math {

    class Pi : public Constant<Pi>
    {
        using base = Constant<Pi>;

    protected:
        std::ostream &print(std::ostream &out) const override
        { return out << "pi"; }

    public:
        using base::base;

        bool Is_pi() const override { return true; }
        bool operator==(const Valuable& v) const override
        { return v.Is_pi(); }
    };

    namespace constant {
        const Pi pi;
    }
}
