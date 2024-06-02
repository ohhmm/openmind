#pragma once
#include <functional>
#include "Goal.h"
#include "Facility.h"

class IdleTimeGoalGenerator;

class WaitForComputerIdle :
	public Goal
{
    typedef Goal    base_t;
    //typedef std::shared_ptr< std::vector<Facility::ptr_t> >       facility_collection_ptr_t;
	enum State {None, RunningProcesses, IdleWork, Stopped};
    volatile State _state;

    void ProcessStartedNotification(bool shouldCreateGoal);

    void ReachNone();
    void ReachIdleWork();
    void DoNothing();
    bool Archivable() override; // FIXME : Should not be archivable

public:
    WaitForComputerIdle( GoalGenerator::ptr_t parent );

    bool        Reach() override;
    void		GetResult(void*) override;
	void*		GetResult() override;

    string_t	Name() override;
};

#include "IdleTimeGoalGenerator.h"
