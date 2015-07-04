#pragma once
#include "GoalGenerator.h"
#include <vector>

class GeneratorProxy
	: public GoalGenerator
{
	typedef GoalGenerator	base_t;

protected:
	typedef std::vector<GoalGenerator::ptr_t> goal_generator_collection_t;

	GeneratorProxy(string_t::pointer name);
	virtual ~GeneratorProxy(void);

public:
	typedef std::shared_ptr<GeneratorProxy> ptr_t;

	static GoalGenerator::ptr_t Make(const goal_generator_collection_t& goalGeneratorCollection);

};
