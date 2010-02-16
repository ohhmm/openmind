#include "StdAfx.h"
#include "GoalGenerator.h"

GoalGenerator::GoalGenerator(string_t::const_pointer name)
: base_t(name)
{
}

GoalGenerator::~GoalGenerator(void)
{
}

void GoalGenerator::Bind( adder_t adder )
{
	adders_.push_back(adder);
}

void GoalGenerator::Unbind( adder_t adder )
{
	assert(!"implemented");
	//adders_.remove(
	//adders_.find(adder) );
}