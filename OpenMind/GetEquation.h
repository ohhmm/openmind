#pragma once
#include "GeneralGoalGenerator.h"

class GetEquation :
	public GeneralGoalGenerator<GetEquation>
{
	typedef GeneralGoalGenerator<GetEquation> base_t;
public:
	using GeneralGoalGenerator::GeneralGoalGenerator;
	GetEquation(const string_t& name) : base_t(name) {}
	~GetEquation(void);
	Goal::ptr_t GenerateGoal();
};
