#pragma once
#include "goalgenerator.h"

class GetEquation :
	public GoalGenerator
{
public:
	GetEquation(void);
	~GetEquation(void);
	Goal::ptr_t GenerateGoal();
};
