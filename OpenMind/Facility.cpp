#include "stdafx.h"
#include <boost/bind.hpp>
#include <thread>
#include "Facility.h"

namespace
{
}

Facility::Facility(void)
{
}

Facility::~Facility(void)
{
}

bool Facility::Use( goal_ptr_t goal )
{
	assert(!"implemented");
	return true;
}

std::list<Facility::goal_ptr_t> Facility::ToUse()
{
	std::list<goal_ptr_t> goals;
	return goals;
}

bool Facility::Invoke()
{
    assert(!"implemented");
    return true;
}

void Facility::AsyncInvoke( result_notification_f callbackNotifyResult )
{
    std::thread(
        boost::bind( &Facility::CbResultFunctionThread, this, callbackNotifyResult) );
}

void Facility::CbResultFunctionThread( boost::function< void(bool) > callbackNotifyResult )
{
    callbackNotifyResult( Invoke() );
}

bool Facility::TryShutdown()
{
    assert(!"implemented");
    return false;
}

void Facility::ForceShutdown()
{
    assert(!"implemented");
}