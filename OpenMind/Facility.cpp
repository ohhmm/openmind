#include "StdAfx.h"
#include "Facility.h"

Facility::Facility(void)
{
}

Facility::~Facility(void)
{
}

bool Facility::Use( goal_ptr_t goal )
{
	assert(!"implemented");
	return true;
}

std::list<Facility::goal_ptr_t> Facility::ToUse()
{
	std::list<goal_ptr_t> goals;
	return goals;
}