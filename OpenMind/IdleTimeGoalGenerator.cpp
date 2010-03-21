#include "StdAfx.h"
#include "WaitForComputerIdle.h"
#include "IdleTimeGoalGenerator.h"

IdleTimeGoalGenerator::IdleTimeGoalGenerator(void)
: base_t(L"IdleTimeGoalGenerator")
{
}

IdleTimeGoalGenerator::~IdleTimeGoalGenerator(void)
{
}

Goal::ptr_t IdleTimeGoalGenerator::GenerateGoal()
{
    mutex_t::scoped_lock lock(*const_cast<mutex_t*>(&goalPresenseControl_));

    return Goal::ptr_t(new WaitForComputerIdle(facilities_));
}