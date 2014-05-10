#include "stdafx.h"
#include "ConsoleGeneratorProxy.h"

#include "ProportionGenerator.h"
#include "GetEquation.h"

#include <iostream>
//#include <FerrisLoki/loki/Typelist.h>
#include <boost/lexical_cast.hpp>

namespace
{
	typedef GoalGenerator::ptr_t (*GeneratorMaker_t)();

	const GeneratorMaker_t Generators[] =
	{
//		ProportionGenerator::Make,
		GetEquation::Make
	};

//	typedef	Loki::TL::MakeTypelist<
//		ProportionGenerator
//	>::Result generators_tl;


//#define StartTypeList ;\
//#define , <TypeList
	//typedef 

	//Loki::TL::

	//template class T;
	//struct AddGeneratorType
	//{

	//};
}

ConsoleGeneratorProxy::ConsoleGeneratorProxy(void)
: base_t(L"ConsoleGeneratorProxy_Menu")
{
	for(GeneratorMaker_t generator : Generators)
	{
		_generators.push_back(generator());
	}
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

	if (!i)
	{
		exit(0);
	}

	return _generators[i-1]->GenerateGoal();	
}
