//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "Constant.h"

namespace omnn::math {

    class Euler : public Constant<Euler>
    {
        using base = Constant<Euler>;

    protected:
        std::ostream &print(std::ostream &out) const override
        { return out << 'e'; }

    public:
        using base::base;

        bool Is_e() const override { return true; }
        bool operator==(const Valuable& v) const override
        { return v.Is_e(); }

//        bool operator<(const Valuable &v) const override;
    };

    namespace constant {
        const Euler e;
    }
}
