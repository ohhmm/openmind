#pragma once
#include <deque>
#include <set>
#include "GoalGenerator.h"

class Mind
{
	std::set<GoalGenerator::ptr_t> goalGenerators_;
	typedef std::deque<Goal::ptr_t>	goals_collection_t;
	goals_collection_t goals_;
	goals_collection_t richedGoals_;
public:
	Mind(void);
	void AddGoalGenerator(GoalGenerator::ptr_t generator);
	void Run();
	~Mind(void);
};
