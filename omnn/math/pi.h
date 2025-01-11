//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "Constant.h"

namespace omnn::math {

    class Pi : public Constant<Pi>
    {
        using base = Constant<Pi>;

    public:
        using base::base;

        static constinit std::string_view SerializationName;

        bool Is_pi() const override { return true; }
        Valuable IsPositive() const override { return {}; }
        bool operator==(const Valuable& v) const override
        { return v.Is_pi(); }
        
		std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable& v) const override { return {}; }
        std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override { return {}; }
        bool MultiplyIfSimplifiable(const Valuable& v) override { return {}; }

        explicit operator double() const override;
    };

    namespace constant {
        static const Pi pi;
    }
}
