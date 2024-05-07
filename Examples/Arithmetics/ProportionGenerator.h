#pragma once
#include <string>
#include <iostream>
#include <OpenMind/GeneralGoalGenerator.h>

class ProportionGenerator :
	public GeneralGoalGenerator<ProportionGenerator>
{
	typedef GeneralGoalGenerator<ProportionGenerator> base_t;
public:
	explicit ProportionGenerator(string_t::const_pointer name) : base_t(name) {}
	Goal::ptr_t GenerateGoal();

	//Goal::ptr_t GenerateGoal()

};
