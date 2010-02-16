#pragma once
#include "goalgenerator.h"

class GetEquation :
	public GoalGenerator
{
	typedef GoalGenerator base_t;
public:
	GetEquation(void);
	~GetEquation(void);
	Goal::ptr_t GenerateGoal();
};
