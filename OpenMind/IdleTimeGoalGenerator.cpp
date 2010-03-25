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
    Goal::ptr_t goal(new WaitForComputerIdle(static_cast<GoalGenerator::ptr_t>(this))); // FIXME : Wrong use of shared pointer
    goal->SubscribeOnReach( boost::bind(&base_t::Reached, this) );
    Generated();
    return goal;
}

void IdleTimeGoalGenerator::AddFacility( Facility::ptr_t facility )
{
    facilities_->push_back(facility);
}