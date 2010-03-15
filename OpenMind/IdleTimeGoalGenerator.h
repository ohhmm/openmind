#pragma once
#include "goalgenerator.h"
#include <boost/detail/lightweight_mutex.hpp>

class IdleTimeGoalGenerator :
    public GoalGenerator
{
	typedef GoalGenerator base_t;
	volatile bool generated_;

    typedef boost::detail::lightweight_mutex  mutex_t;
    volatile mutex_t goalPresenseControl_;
public:
	IdleTimeGoalGenerator(void);
	~IdleTimeGoalGenerator(void);

	Goal::ptr_t GenerateGoal();
};
