#include "stdafx.h"
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
	lock_t lock(*const_cast<mutex_t*>(&goalPresenseControl_));
    Goal::ptr_t goal(new WaitForComputerIdle(this->shared_from_this()));
    goal->SubscribeOnReach( std::bind(&base_t::Reached, this) );
    Generated();
    return goal;
}

void IdleTimeGoalGenerator::AddFacility( Facility::ptr_t facility )
{
    facilities_->push_back(facility);
}