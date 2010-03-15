#include "StdAfx.h"
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