#include "stdafx.h"
#include "GoalGenerator.h"

GoalGenerator::GoalGenerator(string_t::const_pointer name)
:   base_t(name),
    facilities_(new facility_collection_t())
{
}

GoalGenerator::~GoalGenerator(void)
{
}

void GoalGenerator::Bind( adder_t adder )
{
	adders_.push_back(adder);
}

void GoalGenerator::Bind( Facility::ptr_t facility )
{
    facilities_->push_back(facility);
}

void GoalGenerator::Unbind( adder_t adder )
{
	assert(!"implemented");
	//adders_.remove(
	//adders_.find(adder) );
}

bool GoalGenerator::IsNeedToGenerate()
{
	return true;
}

void GoalGenerator::AddFacility( Facility::ptr_t facility )
{
    facilities_->push_back(facility);
}

void GoalGenerator::PushGoal( Goal::ptr_t goal )
{
    BOOST_FOREACH(adder_t adder, adders_)
    {
        adder(goal);
    }
}

void GoalGenerator::UseFacilities( boost::function<void (Facility::ptr_t)> f )
{
    BOOST_FOREACH(Facility::ptr_t facility, *facilities_)
    {
        f(facility);
    }
}

void GoalGenerator::SubscribeGeneration( need_generate_notification_fn_t fn )
{
	needGenerationNotificators_.push_back(fn);
}
