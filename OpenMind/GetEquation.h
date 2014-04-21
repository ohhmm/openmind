#pragma once
#include "GeneralGoalGenerator.h"

class GetEquation :
	public GeneralGoalGenerator<GetEquation>
{
	typedef GeneralGoalGenerator<GetEquation> base_t;
public:
	using GeneralGoalGenerator::GeneralGoalGenerator;
	//GetEquation(void);
	~GetEquation(void);
	Goal::ptr_t GenerateGoal();
};
