#pragma once
#include "GeneratorProxy.h"

class ConsoleGeneratorProxy :
	public GeneratorProxy
{
	typedef GeneratorProxy	base_t;

	goal_generator_collection_t _generators;
public:
	ConsoleGeneratorProxy(const goal_generator_collection_t& goalGeneratorCollection);
	~ConsoleGeneratorProxy(void);
	Goal::ptr_t GenerateGoal();
};
