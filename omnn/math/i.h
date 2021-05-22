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
        YesNoMaybe IsMultival() const override { return YesNoMaybe::No; }

        bool operator==(const Valuable& v) const override
        { return v.Is_i(); }

        std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable &v) const override;
        bool MultiplyIfSimplifiable(const Valuable& v) override;
    };

    namespace constant {
        static const MinusOneSq i;
    }


}
