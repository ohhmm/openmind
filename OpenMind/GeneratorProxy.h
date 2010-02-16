#pragma once
#include "GoalGenerator.h"

class GeneratorProxy
	: public GoalGenerator
{
	typedef GoalGenerator	base_t;

public:
	typedef boost::shared_ptr<GeneratorProxy> ptr_t;
	static GoalGenerator::ptr_t Make();
protected:
	GeneratorProxy(string_t::pointer name);
	virtual ~GeneratorProxy(void);
};
