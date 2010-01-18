#include "StdAfx.h"
#include "ProportionGenerator.h"
#include "SolveProportion.h"

ProportionGenerator::ProportionGenerator(void)
{
}

Goal::ptr_t ProportionGenerator::GenerateGoal()
{
	std::wcout << L"Specify members and X some one: A / B = C / D " << std::endl;
	std::wstring a, b, c, d;

	std::wcout << L"A = "; std::wcin >> a; std::wcout << std::endl;
	std::wcout << L"B = "; std::wcin >> b; std::wcout << std::endl;
	std::wcout << L"C = "; std::wcin >> c; std::wcout << std::endl;
	std::wcout << L"D = "; std::wcin >> d; std::wcout << std::endl;

	const std::wstring x(L"x");
	assert(a==x||b==x||c==x||d==x);

	return Goal::ptr_t(new SolveProportion(a,b,c,d));
}
