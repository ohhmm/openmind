#include "StdAfx.h"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include "IdleTimeGoalGenerator.h"
#include "WaitForComputerIdle.h"

namespace
{
	const DWORD IdleMillisecondsCount = 5000;//25000;
}


WaitForComputerIdle::WaitForComputerIdle( GoalGenerator::ptr_t parent )
: base_t(parent)
, _state(None)
{ }

bool WaitForComputerIdle::Reach()
{
    typedef void (WaitForComputerIdle::*reach_meth_t)();
    static const reach_meth_t ReachByState[] = {
        &WaitForComputerIdle::ReachNone,
        &WaitForComputerIdle::DoNothing,
        &WaitForComputerIdle::ReachIdleWork,
        &WaitForComputerIdle::DoNothing };

    (this->*(ReachByState[_state]))();
	return _state == Stopped;
}

WaitForComputerIdle::string_t WaitForComputerIdle::Name()
{
    return string_t(L"WaitForComputerIdle");
}

void WaitForComputerIdle::GetResult( void* )
{
    assert(!"Not Implemented");
}

void* WaitForComputerIdle::GetResult()
{
	assert(!"Not Implemented");
	return 0;
}

void WaitForComputerIdle::ProcessStartedNotification( bool procRunSuccess )
{
    if (procRunSuccess)
    {
        _state = IdleWork;
        _result << L"Processes Creation successful" << std::endl;
    }
    else
    {
        _result << L"Processes Creation failed" << std::endl;
    }
}

void WaitForComputerIdle::ReachNone()
{
    LASTINPUTINFO ii = {sizeof(ii),0};
    if (!! GetLastInputInfo(&ii) 
        && (GetTickCount() - ii.dwTime) >= IdleMillisecondsCount)
    {
        Facility::result_notification_f processStartedNotification =
            boost::bind(&WaitForComputerIdle::ProcessStartedNotification, this, _1);

        _parentGenerator->UseFacilities(
            boost::bind(&Facility::AsyncInvoke, _1, processStartedNotification) );

        _state = RunningProcesses;
    }
}

void WaitForComputerIdle::ReachIdleWork()
{
    LASTINPUTINFO ii = {sizeof(ii),0};
    if (!! GetLastInputInfo(&ii) 
        && (GetTickCount() - ii.dwTime) < IdleMillisecondsCount)
    {
        bool stopped = true;
        struct TryStop 
        {
            bool& stopped_;
            TryStop(bool& stopped)
                : stopped_(stopped)
            {}
            void operator()(Facility::ptr_t f)
            {
                stopped_ = stopped_ && f->TryShutdown();
            }
        };
        
        TryStop tryStop(stopped);
        _parentGenerator->UseFacilities(tryStop);
        if (!tryStop.stopped_)
        {
            _parentGenerator->UseFacilities(
                boost::bind(&Facility::ForceShutdown, _1));

        }
                
        _result << L"Workers are stopped" << std::endl;

        _state = Stopped;
		base_t::Reach();
    }
}

void WaitForComputerIdle::DoNothing()
{

}

bool WaitForComputerIdle::Archivable()
{
    return true;
}