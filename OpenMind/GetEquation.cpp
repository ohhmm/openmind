#include "StdAfx.h"
#include <iostream>
#include "GetEquation.h"

GetEquation::GetEquation(void)
{
}

GetEquation::~GetEquation(void)
{
}

Goal::ptr_t GetEquation::GenerateGoal()
{
	std::wstring s;
	std::wcin >> s;
	return Goal::ptr_t();
}