#pragma once
#include <list>
#include "Goal.h"

class Goal;
class Facility
{
public:
	typedef boost::shared_ptr<Goal> goal_ptr_t;
	Facility(void);
	virtual bool Use(goal_ptr_t goal);
	virtual std::list<goal_ptr_t> ToUse();
	virtual ~Facility(void);
};
