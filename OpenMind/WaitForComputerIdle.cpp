#include "StdAfx.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "WaitForComputerIdle.h"

namespace
{
	const DWORD IdleMillisecondsCount = 25000;
    void None(bool)    {    }
}

//WaitForComputerIdle::WaitForComputerIdle( boost::function<void()> cmd ) : command_(cmd)
//{
//
//}

WaitForComputerIdle::WaitForComputerIdle( facility_collection_ptr_t facilities )
: facilities_(facilities)
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
        for_each(facilities_->begin(), facilities_->end(), 
            boost::bind(&Facility::AsyncInvoke, _1, None) );
	}

	return reached;
}

WaitForComputerIdle::string_t WaitForComputerIdle::Name()
{
    return string_t(L"WaitForComputerIdle");
}

void WaitForComputerIdle::GetResult( void* )
{
    assert(!"Not Implemented");
}

WaitForComputerIdle::string_t WaitForComputerIdle::SerializedResult()
{
    assert(!"Not Implemented");
    return string_t();
}