#pragma once
#include "GoalGenerator.h"

class SolveEquation :
	public Goal
{
	std::wstring equation_;
public:
	SolveEquation(const std::wstring& equation);
	~SolveEquation(void);
};
