//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "PositiveConstant.h"

namespace omnn::math {

    class Pi : public PositiveConstant<Pi>
    {
        using base = PositiveConstant<Pi>;

    public:
        using base::base;

        static constinit std::string_view SerializationName;

        [[nodiscard]]
        constexpr bool Is_pi() const override { return true; }

        [[nodiscard]]
        bool operator<(const Valuable&) const override;

        [[nodiscard]]
        Valuable MustBeInt() const override {
            return Valuable(std::static_pointer_cast<Valuable>(std::make_shared<Integer>(-1)));
        }
        Valuable IsPositive() const override { return {}; }
        Valuable Sign() const override;
        
		std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable& v) const override { return {}; }
        std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override { return {}; }
        bool MultiplyIfSimplifiable(const Valuable& v) override { return {}; }

        explicit operator double() const override;
    };

    namespace constant {
        static const Pi pi;
    }
}
