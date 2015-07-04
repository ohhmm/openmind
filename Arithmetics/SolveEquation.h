#pragma once
#include <string>
#include <OpenMind/Goal.h>

class SolveEquation :
	public Goal
{
	std::wstring equation_;
public:
	SolveEquation(const std::wstring& equation);
	~SolveEquation(void);
};
