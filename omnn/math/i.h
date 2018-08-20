//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "Constant.h"

namespace omnn::math {

    class MinusOneSq : public Constant<MinusOneSq>
    {
        using base = Constant<MinusOneSq>;

    protected:
        std::ostream &print(std::ostream &out) const override
        { return out << 'i'; }

    public:
        using base::base;

        bool Is_i() const override { return true; }
        bool operator==(const Valuable& v) const override
        { return v.Is_i(); }
    };

    namespace constant {
        const MinusOneSq i;
    }
}
