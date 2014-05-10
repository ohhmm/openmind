#include "stdafx.h"
#include <algorithm>
#include <mutex>
#include <functional>
#include <chrono>
#include <thread>


#include "Mind.h"

Mind::Mind(void) :
		interval_(0) {
}

Mind::~Mind(void) {
}

void Mind::Run() {
	for (auto generator : goalGenerators_) {
		generator->SubscribeGeneration([&](GoalGenerator::ptr_t goalGenerator) {
			if (goalGenerator->IsNeedToGenerate()) {
				auto goal = goalGenerator->GenerateGoal();
				goals_.push_back(goal);
				goal->StartReachingAsync();
			}
		});
	}

	while (!ReachGoals() || goals_.empty()) {
		if (interval_) {
			std::chrono::milliseconds duration(interval_);
			std::this_thread::sleep_for(duration);
		}
	}
}

void Mind::Run(Goal::ptr_t goal) {
	goals_.push_back(goal);
	Run();
}

void Mind::AddGoalGenerator(GoalGenerator::ptr_t generator) {
	generator->Bind([this](Goal::ptr_t goal) {goals_.push_back(goal);});
	goalGenerators_.insert(generator);
}

bool Mind::ReachGoals() {
	bool someRiched = false;

	if (goals_.size()) {
		std::vector<goals_collection_t::iterator> toErase;
		for (goals_collection_t::iterator it = goals_.begin();
				it != goals_.end(); ++it) {
			Goal::ptr_t goal = *it;
			bool isReached = std::find(richedGoals_.begin(), richedGoals_.end(),
					goal) != richedGoals_.end();
			someRiched = isReached || goal->Reach();
			if (someRiched) {
				if (!isReached) // it is reached by now
				{
					if ((*it)->Archivable()) {
						richedGoals_.push_front(*it);
					}

					std::wcout << std::endl << L"Goal " << (*it)->Name()
							<< L" reached. Result: "
							<< (*it)->SerializedResult() << std::endl;
				}

				toErase.push_back(it);
			} else {
				Goal::container_t newGoals = goal->ToReach();
				goals_.insert(it, newGoals.begin(), newGoals.end());
			}
		}

		for_each(toErase.begin(), toErase.end(),
				[this](goals_collection_t::iterator goal) {goals_.erase(goal);});
	}

	return someRiched;
}

void Mind::SetInterval(int msec) {
	interval_ = msec;
}

void Mind::AddGoal(Goal::ptr_t goal) {
	goals_.push_front(goal);
}
