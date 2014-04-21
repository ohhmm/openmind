#include "stdafx.h"
#include <iostream>
#include "GetEquation.h"

//GetEquation::GetEquation(void)
//: base_t(L"Generate Equation")
//{
//}

GetEquation::~GetEquation(void)
{
}

Goal::ptr_t GetEquation::GenerateGoal()
{
	std::wstring s;
	std::wcin >> s;
	return Goal::ptr_t();
}
