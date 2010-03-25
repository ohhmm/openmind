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
	while(!ReachGoals() || goals_.empty())
	{
		BOOST_FOREACH(GoalGenerator::ptr_t generator, goalGenerators_)
		{
			if (generator->IsNeedToGenerate())
			{
				goals_.push_back(
					generator->GenerateGoal() );
			}
		}
	}
}

void Mind::AddGoalGenerator( GoalGenerator::ptr_t generator )
{
	generator->Bind(
		boost::bind(&goals_collection_t::push_back, goals_, _1 ));

	goalGenerators_.insert(generator);
}

bool Mind::ReachGoals()
{
	bool someRiched = false;

	if (goals_.size())
	{
		std::vector<goals_collection_t::iterator> toErase;
		for( goals_collection_t::iterator it = goals_.begin();
			it != goals_.end(); ++it )
		{
			Goal::ptr_t goal = *it;
			bool isRiched = std::find(richedGoals_.begin(), richedGoals_.end(), goal) != richedGoals_.end();
			someRiched = isRiched || goal->Reach();
			if (someRiched)
			{
				if (!isRiched) // it is reached by now
				{
                    (*it)->OnReach();

                    if ((*it)->Archivable())
                    {
                        richedGoals_.push_front(*it);
                    }
					
					std::wcout
						<< std::endl << L"Goal " << (*it)->Name() 
						<< L" reached. Result: " << (*it)->SerializedResult() << std::endl;
				}

				toErase.push_back(it);
			}
			else
			{
				Goal::container_t newGoals = goal->ToReach();
				goals_.insert( it, newGoals.begin(), newGoals.end() ); 
			}
		}

		for_each(toErase.begin(), toErase.end(), 
			boost::bind(&goals_collection_t::erase, &goals_, _1) );
	}

	return someRiched;
}
