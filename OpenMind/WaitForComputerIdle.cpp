#include "stdafx.h"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <functional>
#include "IdleTimeGoalGenerator.h"
#include "WaitForComputerIdle.h"

namespace
{
	const unsigned int IdleMillisecondsCount = 5000;//25000;
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

#ifdef _WIN32
bool getIdleMsec(unsigned& idleTime) {
	LASTINPUTINFO ii = {sizeof(ii),0};
	bool result = !! GetLastInputInfo(&ii);
	if(result)
		idleTime = GetTickCount() - ii.dwTime;
	return result;
}
#else
# include <X11/extensions/scrnsaver.h>
bool getIdleMsec(unsigned& idleTime) {
	XScreenSaverInfo info;
    Display *dpy = XOpenDisplay(NULL);
	bool result = dpy && XScreenSaverQueryInfo(dpy,RootWindow(dpy, DefaultScreen(dpy)),&info);
	if(result)
		idleTime = info.idle; //msec
	return result;
}
unsigned int getIdleTime() {
	unsigned int result = (unsigned int)-1;

	return result;
}
#endif

void WaitForComputerIdle::ReachNone()
{
	unsigned msec;
	if (getIdleMsec(msec) && msec >= IdleMillisecondsCount)
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
	unsigned msec;
    if (getIdleMsec(msec) && msec < IdleMillisecondsCount)
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
