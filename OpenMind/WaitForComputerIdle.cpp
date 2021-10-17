#include "stdafx.h"
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

//#elif __APPLE__
//#include <CoreGraphics/CoreGraphics.h>
//bool getIdleMsec(unsigned& idleTime) {
//    throw "Implement!";
//    return true;
//}

#elif __has_include(<X11/extensions/scrnsaver.h>)
# include <X11/extensions/scrnsaver.h>
bool getIdleMsec(unsigned& idleTime) {
	XScreenSaverInfo info;
    Display *dpy = XOpenDisplay(":0");
	bool result = dpy && XScreenSaverQueryInfo(dpy,RootWindow(dpy, DefaultScreen(dpy)),&info);
	if(result)
		idleTime = info.idle; //msec
	return result;
}
unsigned int getIdleTime() {
	unsigned int result = (unsigned int)-1;

	return result;
}
#else
bool getIdleMsec(unsigned& idleTime) {
    return {};
}
#endif

void WaitForComputerIdle::ReachNone()
{
	unsigned msec;
    auto success = getIdleMsec(msec);
    assert(success);
	if (success && msec >= IdleMillisecondsCount)
    {
        Facility::result_notification_f processStartedNotification =
            std::bind(&WaitForComputerIdle::ProcessStartedNotification, this, std::placeholders::_1);

		_state = RunningProcesses;

		_parentGenerator->UseFacilities([&](Facility::ptr_t f) {f->AsyncInvoke(processStartedNotification); });
    }
}

void WaitForComputerIdle::ReachIdleWork()
{
	unsigned msec;
    if (getIdleMsec(msec) && msec < IdleMillisecondsCount)
    {
        bool stopped = true;

		_parentGenerator->UseFacilities(
			[&](Facility::ptr_t f){ stopped = stopped && f->TryShutdown(); });

		if (!stopped)
            _parentGenerator->UseFacilities(
				[] (Facility::ptr_t facility) { facility->ForceShutdown(); }
			);
                
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
