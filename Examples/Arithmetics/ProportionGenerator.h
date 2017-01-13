#pragma once
#include <string>
#include <iostream>
#include <OpenMind/GeneralGoalGenerator.h>

class ProportionGenerator :
	public GeneralGoalGenerator<ProportionGenerator>
{
	typedef GeneralGoalGenerator<ProportionGenerator> base_t; 
public:
	ProportionGenerator(string_t::const_pointer name);
	Goal::ptr_t GenerateGoal();

	//Goal::ptr_t GenerateGoal()

};
