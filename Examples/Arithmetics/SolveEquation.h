#pragma once
#include <string>
#include <OpenMind/Goal.h>

class SolveEquation :
	public Goal
{
	std::wstring equation_;

public:
	explicit SolveEquation(const std::wstring& equation);
	~SolveEquation(void);

	bool Reach() { return true; }
    string_t Name() { return equation_; }
};
