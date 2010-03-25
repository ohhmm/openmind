#pragma once
#include "goalgenerator.h"

template <class SelfT>
class GoalGeneratorBase :
    public GoalGenerator
{
    typedef GoalGenerator base_t;
public:
    // default maker
    static GoalGenerator::ptr_t Make()
    {
        return GoalGenerator::ptr_t(new SelfT());
    }

    GoalGeneratorBase(string_t::pointer name)
        : base_t(name)
    {
    }
};
