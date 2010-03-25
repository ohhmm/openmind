#pragma once
#include "SingletonGoalGenerator.h"
#include "WaitForComputerIdle.h"
#include <boost/detail/lightweight_mutex.hpp>

class IdleTimeGoalGenerator :
    public SingletonGoalGenerator<IdleTimeGoalGenerator>
{
	typedef SingletonGoalGenerator<IdleTimeGoalGenerator>  base_t;
public:
	IdleTimeGoalGenerator(void);
	~IdleTimeGoalGenerator(void);

	Goal::ptr_t GenerateGoal();

    void AddFacility(Facility::ptr_t facility);
};
