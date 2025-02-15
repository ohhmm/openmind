//
// Created by Sergej Krivonos on 5/26/18.
//

#pragma once
#include "Constant.h"

namespace omnn::math {

    class MinusOneSurd : public Constant<MinusOneSurd>
    {
        using base = Constant<MinusOneSurd>;

    public:
        using base::base;

        static constinit std::string_view SerializationName;

        bool Is_i() const override { return true; }
        bool IsSimple() const noexcept override { return {}; }
        YesNoMaybe IsMultival() const noexcept override { return YesNoMaybe::No; }       

        bool operator==(const Valuable& v) const override
        { return v.Is_i(); }

        Valuable& operator^=(const Valuable&) override;

		std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable& v) const override;
        std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable &v) const override;
        bool MultiplyIfSimplifiable(const Valuable& v) override;
        
        Valuable& sq() override;
        Valuable Sq() const override;
        Valuable Sqrt() const override;
		Valuable Sign() const override;
        Valuable abs() const override;
    };

    namespace constant {
        static const MinusOneSurd i;
    }


}
