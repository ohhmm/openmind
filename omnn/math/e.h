//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "Constant.h"

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

        Valuable& sq() override;
        Valuable Sq() const override;

        //        bool operator<(const Valuable &v) const override;

		std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable& v) const;
        std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const;
        bool MultiplyIfSimplifiable(const Valuable& v);
    };

    namespace constant {
        static const Euler e;
    }
}
