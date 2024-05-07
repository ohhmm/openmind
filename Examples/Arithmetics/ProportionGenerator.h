#pragma once
#include <string>
#include <iostream>
#include <OpenMind/GeneralGoalGenerator.h>

class ProportionGenerator :
	public GeneralGoalGenerator<ProportionGenerator>
{
	typedef GeneralGoalGenerator<ProportionGenerator> base_t;
public:
	using base_t::base_t;
	Goal::ptr_t GenerateGoal();

	//Goal::ptr_t GenerateGoal()

};
