#include "stdafx.h"
#include "ConsoleGeneratorProxy.h"


#include <iostream>
//#include <FerrisLoki/loki/Typelist.h>
#include <boost/lexical_cast.hpp>

ConsoleGeneratorProxy::ConsoleGeneratorProxy(const goal_generator_collection_t& goalGeneratorCollection)
: base_t(L"ConsoleGeneratorProxy_Menu")
, _generators(goalGeneratorCollection)
{
}

ConsoleGeneratorProxy::~ConsoleGeneratorProxy(void)
{
}

Goal::ptr_t ConsoleGeneratorProxy::GenerateGoal()
{
	std::wcout << L"Here is list of available goal generators:" << std::endl;
	std::wcout << L"\t0. Exit" << std::endl;
	size_t generatorsCount = _generators.size();
	size_t i = 0 ;
	for ( ; i < generatorsCount; ++i)
	{
		std::wcout
			<< L'\t' << boost::lexical_cast<std::wstring>(i+1) << L". "
			<< _generators[i]->Name() << std::endl;
	}
	std::wcout << L"choose: ";

	do
	{
		i = boost::lexical_cast<size_t>(std::cin.get()) - '0';
	} while (i && i > generatorsCount);

	if (i==0)
	{
		exit(0);
	}

	return _generators[i-1]->GenerateGoal();	
}
