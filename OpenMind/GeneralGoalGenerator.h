#pragma once
#include "goalgenerator.h"

template <class ChildT>
class GeneralGoalGenerator :
	public GoalGenerator
{
public:

	typedef GoalGenerator base_t;
	typedef boost::shared_ptr<ChildT>	ptr_t;

	GeneralGoalGenerator(void)
	{
	}

	~GeneralGoalGenerator(void)
	{
	}

	static base_t::ptr_t Make()
	{
		return base_t::ptr_t(new ChildT());
	}
};
