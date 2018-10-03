//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include "Exponentiation.h"
#include "Modulo.h"
#include "Sum.h"
#include "Product.h"

namespace omnn::math {

    template <class Chld>
    class ValuableDescendant
        : public ValuableDescendantContract<Chld>
    {
        using base = ValuableDescendantContract<Chld>;

        Valuable &operator+=(const Valuable &v) override
        {
            return base::Become(Sum{*this,v});
        }

        Valuable &operator*=(const Valuable &v) override
        {
            return base::Become(Product{*this,v});
        }

        Valuable &operator/=(const Valuable &v) override
        {
            return base::Become(Fraction(*this,v));
        }

        Valuable &operator%=(const Valuable &v) override
        {
            return base::Become(Modulo(*this,v));
        }

        Valuable &operator^=(const Valuable& v) override
        {
            return base::Become(Exponentiation(*this,v));
        }
    };
}
