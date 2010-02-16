#pragma once
#include <vector>
#include <boost/function.hpp>
#include <boost/any.hpp>
#include "Goal.h"
#include "Named.h"

class GoalGenerator
	: public Named
{
	typedef Named			base_t;
	typedef GoalGenerator	self_t;
	typedef boost::function<void (Goal::ptr_t)>  adder_t;
	std::vector<adder_t>	adders_;

protected:
	GoalGenerator(string_t::const_pointer name);

public:
	typedef boost::shared_ptr<self_t> ptr_t;

	virtual ~GoalGenerator(void);

	void Bind(adder_t adder);
	void Unbind(adder_t adder);

	virtual Goal::ptr_t GenerateGoal() = 0;
};
