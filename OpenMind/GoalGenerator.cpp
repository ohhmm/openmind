#include "StdAfx.h"
#include "GoalGenerator.h"

GoalGenerator::GoalGenerator(void)
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