#pragma once
#include <deque>
#include <set>
#include "GoalGenerator.h"

class Mind
{
	std::set<GoalGenerator::ptr_t> goalGenerators_;

	typedef std::list<Goal::ptr_t>	goals_collection_t;
	goals_collection_t goals_;
	goals_collection_t richedGoals_;

	std::vector<void*> results_;

	int interval_;

public:
	Mind();

	void AddGoalGenerator(GoalGenerator::ptr_t generator);
	void Run();
	void Run(Goal::ptr_t goal);

	void SetInterval(int msec);

	bool ReachGoals();
	~Mind(void);
	void AddGoal( Goal::ptr_t goal );
};
