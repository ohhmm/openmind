#include "StdAfx.h"
#include "WaitForComputerIdle.h"

namespace
{
	const DWORD IdleMillisecondsCount = 25000;
}

WaitForComputerIdle::WaitForComputerIdle( boost::function<void()> cmd ) : command_(cmd)
{

}
WaitForComputerIdle::~WaitForComputerIdle(void)
{
}

bool WaitForComputerIdle::Reach()
{
	// return true if user didn't access computer for a minute by keyboard&mouse
	LASTINPUTINFO ii;
	bool reached = 
		GetLastInputInfo(&ii) &&
		ii.dwTime > IdleMillisecondsCount;

	if (reached)
	{
		command_();
	}

	return reached;
}