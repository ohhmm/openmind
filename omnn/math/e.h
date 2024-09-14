//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "Constant.h"
#include <numbers>

namespace omnn::math {

    class Euler : public Constant<Euler>
    {
        using base = Constant<Euler>;

    public:
        using base::base;

        static constinit std::string_view SerializationName;

        bool Is_e() const override { return true; }
        bool operator==(const Valuable& v) const override
        { return v.Is_e(); }
        
        explicit
        constexpr operator double() const override { return std::numbers::e_v<double>; }

        explicit
        constexpr operator long double() const override { return std::numbers::e_v<long double>; }

        Valuable& sq() override;
        Valuable Sq() const override;

        //        bool operator<(const Valuable &v) const override;

		std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable& v) const override;
        std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;
        bool MultiplyIfSimplifiable(const Valuable& v) override;
    };

    namespace constant {
        static const Euler e;
    }
}
