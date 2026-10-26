#pragma once
#include "Constant.h"

namespace omnn::math {

template <class Child>
    class PositiveConstant : public Constant<Child>
{
    using base = Constant<Child>;

public:
    using base::base;

    [[nodiscard]]
    Valuable IsPositive() const override {
        return {};
    }

    [[nodiscard]]
    Valuable Sign() const override {
        return Valuable(std::static_pointer_cast<Valuable>(std::make_shared<Integer>(1)));
    }
};
}
