#pragma once

#include <OpenMind/GeneralGoalGenerator.h>

class GetEquation :
	public GeneralGoalGenerator<GetEquation>
{
	typedef GeneralGoalGenerator<GetEquation> base_t;
public:
//	using base_t::GeneralGoalGenerator;
	explicit GetEquation(const string_t& name) : base_t(name) {}
	~GetEquation(void);
	Goal::ptr_t GenerateGoal();
};
