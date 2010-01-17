#include "StdAfx.h"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include "Mind.h"

Mind::Mind(void)
{
}

Mind::~Mind(void)
{
}

void Mind::Run()
{
	do
	{
		//BOOST_FOREACH(Goal::ptr_t goal, goals_)
		for(goals_collection_t::iterator it = goals_.begin(); it != goals_.end(); ++it)
		{
			Goal::ptr_t goal = *it;
			bool isRiched = std::find(richedGoals_.begin(), richedGoals_.end(), goal) != richedGoals_.end();
			if (isRiched || goal->Rich() )
			{
				if (!isRiched) // it is riched by now
				{
					richedGoals_.insert( richedGoals_.end(), goal);
				}

				goals_.erase( it );
			}
			else
			{
				Goal::container_t newGoals = goal->ToRich();
				goals_.insert( goals_.end(), newGoals.begin(), newGoals.end() ); 
			}
		}

		//if (!goals_.empty())
		{
			std::for_each(goalGenerators_.begin(), goalGenerators_.end(), 
				boost::bind(&GoalGenerator::GenerateGoal, _1) );
		}
	}
	while (!goals_.empty());
}

void Mind::AddGoalGenerator( GoalGenerator::ptr_t generator )
{
	generator->Bind(
		boost::bind(&goals_collection_t::push_back, goals_, _1 ));

	goalGenerators_.insert(generator);
}
