#include "StdAfx.h"
#include "SolveProportion.h"
#include <locale>
#include <boost/lexical_cast.hpp>


namespace
{
	template <class StrT>
	StrT& Prepare(StrT& s)
	{
		static const std::locale l;
		std::for_each(s.begin(), s.end(), 
			boost::bind(&std::tolower<StrT::value_type>, _1, l));
		return s;
	}
}

SolveProportion::SolveProportion(std::wstring a,std::wstring b, std::wstring c, std::wstring d)
{
	Init(Prepare(a), Prepare(b), Prepare(c), Prepare(d));
}

SolveProportion::~SolveProportion(void)
{
}

bool SolveProportion::Init(string_t& a, string_t b, string_t c, string_t d)
{
	bool initialized = a == L"x";
	if (initialized)
	{
		_result = boost::lexical_cast<result_t>(b) * boost::lexical_cast<result_t>(c) / boost::lexical_cast<result_t>(d);  
	}
	else
	{
		initialized = Init(c,d,a,b) || Init(b,a,d,c);
	}

	return initialized;
}
