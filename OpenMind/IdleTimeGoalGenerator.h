#pragma once
#include "goalgenerator.h"
#include <loki/LevelMutex.h>

class IdleTimeGoalGenerator :
	public GoalGenerator
{
	typedef GoalGenerator base_t;
	volatile bool generated_;
	Loki::LevelMutex<
public:
	IdleTimeGoalGenerator(void);
	~IdleTimeGoalGenerator(void);

	Goal::ptr_t GenerateGoal()
};
