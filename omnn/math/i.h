//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "Constant.h"

namespace omnn::math {

    class MinusOneSq : public Constant<MinusOneSq>
    {
        using base = Constant<MinusOneSq>;

    public:
        using base::base;

        static constinit std::string_view SerializationName;

        bool Is_i() const override { return true; }
        YesNoMaybe IsMultival() const override { return YesNoMaybe::No; }

        bool operator==(const Valuable& v) const override
        { return v.Is_i(); }

		std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable& v) const override;
        std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable &v) const override;
        bool MultiplyIfSimplifiable(const Valuable& v) override;
        
        Valuable& sq() override;
        Valuable Sq() const override;

		Valuable Sign() const override;
    };

    namespace constant {
        static const MinusOneSq i;
    }


}
