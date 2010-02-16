#pragma once
#include "generatorproxy.h"

class ConsoleGeneratorProxy :
	public GeneratorProxy
{
	typedef GeneratorProxy	base_t;

	std::vector<GoalGenerator::ptr_t> _generators;
public:
	ConsoleGeneratorProxy(void);
	~ConsoleGeneratorProxy(void);
	Goal::ptr_t GenerateGoal();
};
