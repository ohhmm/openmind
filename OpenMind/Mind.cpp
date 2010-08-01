#include "StdAfx.h"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include "Mind.h"
#include "RiseEventFn.h"

Mind::Mind(void)
: interval_(0)
{
}

Mind::~Mind(void)
{
}

void Mind::Run()
{
	//boost::mutex generationNeed;
	enum {GenerationNeedEventIndex, RichGoalNeedEventIndex};
	HANDLE events[] = { 
		CreateEvent(0,0,false,0),
		CreateEvent(0,0,false,0) };

	bool someNeedToGenerateGoal = true;
	while(!ReachGoals() || goals_.empty())
	{
		bool generationNeeded = false;
		if (someNeedToGenerateGoal)
		{
			BOOST_FOREACH(GoalGenerator::ptr_t generator, goalGenerators_)
			{
				if (generator->IsNeedToGenerate())
				{
					generationNeeded = true;
					goals_.push_back(
						generator->GenerateGoal() );
				}
			}
			if(!generationNeeded)
			{
				someNeedToGenerateGoal = false;
				BOOST_FOREACH(GoalGenerator::ptr_t generator, goalGenerators_)
				{
					struct AssignFn 
					{
						bool& target_;
						HANDLE syncObj_;
					public:
						AssignFn(bool& target, HANDLE m)
							: target_(target), syncObj_(m)
						{ }

						void operator()()
						{
							target_ = true;
							PulseEvent(syncObj_);
						}
					};

					generator->SubscribeGeneration(
						AssignFn(someNeedToGenerateGoal, events[GenerationNeedEventIndex]));
				}
			}
		}
		else
		{
			BOOST_FOREACH(Goal::ptr_t goal, goals_)
			{
				goal->SubscribeOnReach(RiseEventFn(events[RichGoalNeedEventIndex]));
				goal->StartReachingAsync();
			}

			WaitForMultipleObjects(_countof(events), events, false, INFINITE);
		}

		if (interval_)
		{
			Sleep(interval_);
		}
	}
}

void Mind::Run( Goal::ptr_t goal )
{
	goals_.push_back(goal);
	Run();
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

void Mind::SetInterval( int msec )
{
	interval_ = msec;
}

void Mind::AddGoal( Goal::ptr_t goal )
{
	goals_.push_front(goal);
}
