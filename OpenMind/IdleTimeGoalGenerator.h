#pragma once
#include "goalgeneratorbase.h"
#include <boost/detail/lightweight_mutex.hpp>

class IdleTimeGoalGenerator :
    public GoalGeneratorBase<IdleTimeGoalGenerator>
{
	typedef GoalGeneratorBase<IdleTimeGoalGenerator>  base_t;
public:
	IdleTimeGoalGenerator(void);
	~IdleTimeGoalGenerator(void);

	Goal::ptr_t GenerateGoal();
};
