//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include "Valuable.h"

namespace omnn{
namespace extrapolator {

    template <class Chld>
    class ValuableDescendantContract
            : public Valuable
    {
        using self = ValuableDescendantContract;
        friend Chld;
        friend self;
    protected:
        Valuable* Clone() const override
        {
            return new Chld(*static_cast<const Chld*>(this));
        }

    public:
        // once compiler allow
        // todo :
        //ValuableDescendantContract() : Valuable<>() {}
        // instead of
        ValuableDescendantContract() : Valuable(ValuableDescendantMarker()) {}
        ValuableDescendantContract(ValuableDescendantContract&& c) : Valuable(c, ValuableDescendantMarker()) {}
        ValuableDescendantContract(const ValuableDescendantContract& c) : Valuable(c, ValuableDescendantMarker()) {}
        ValuableDescendantContract& operator=(const ValuableDescendantContract& f) { hash=f.hash; return *this; }
        ValuableDescendantContract& operator=(ValuableDescendantContract&& f) { hash=f.hash; return *this; }
        static const Chld* cast(const Valuable& v){
            return Valuable::cast<Chld>(v);
        }

        Valuable abs() const override
        {
            auto i = const_cast<Chld*>(cast(*this));
            if(*i < 0_v)
            {
                return *cast(-*i);
            }
            return *this;
        }
        void optimize() override { }
        Valuable sqrt() const override { throw "Implement!"; }
    };
}}
