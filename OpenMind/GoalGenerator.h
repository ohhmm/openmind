#pragma once
#include <vector>
#include <boost/function.hpp>
#include <boost/any.hpp>
#include "Goal.h"

class GoalGenerator
{
	typedef GoalGenerator self_t;
	typedef boost::function<void (Goal::ptr_t)>  adder_t;
	std::vector<adder_t> adders_;

protected:
	GoalGenerator(void);

public:
	typedef boost::shared_ptr<self_t> ptr_t;

	virtual ~GoalGenerator(void);

	void Bind(adder_t adder);
	void Unbind(adder_t adder);

	virtual Goal::ptr_t GenerateGoal() = 0;
};
