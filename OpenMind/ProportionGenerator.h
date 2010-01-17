#pragma once
#include <string>
#include <iostream>
#include "GeneralGoalGenerator.h"

class ProportionGenerator :
	public GeneralGoalGenerator<ProportionGenerator>
{
public:
	ProportionGenerator(void);
	Goal::ptr_t GenerateGoal();

	//Goal::ptr_t GenerateGoal()

};
