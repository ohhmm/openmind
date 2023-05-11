#pragma once

#include <OpenMind/Goal.h>
#include <omnn/math/Valuable.h>

class SolveProportion :
	public Goal
{
public:
    typedef omnn::math::Valuable result_t;
	typedef std::wstring	string_t;
	SolveProportion(std::wstring a, std::wstring b, std::wstring c, std::wstring d);

	bool Init(string_t& a, string_t b, string_t c, string_t d);

	//Goal 
	bool Reach()				{ return true; }
	void GetResult(void* res)	{ res = &_result; }
	void* GetResult();

	string_t SerializedResult();
	string_t Name() { return _name; }

private:
	result_t _result;
	string_t _name;

};
