#pragma once
#include "SingletonGoalGenerator.h"
#include "WaitForComputerIdle.h"
#include <mutex>

class IdleTimeGoalGenerator :
    public SingletonGoalGenerator<IdleTimeGoalGenerator>
{
	typedef SingletonGoalGenerator<IdleTimeGoalGenerator>  base_t;
public:
	IdleTimeGoalGenerator(void);
	~IdleTimeGoalGenerator(void);

	Goal::ptr_t GenerateGoal() override;

    void AddFacility(Facility::ptr_t facility);
};
