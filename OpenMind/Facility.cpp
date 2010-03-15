#include "StdAfx.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>
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

void Facility::AsyncInvoke( boost::function< void(bool) > callbackNotifyResult )
{
    boost::thread(
        boost::bind( &Facility::CbResultFunctionThread, this, callbackNotifyResult) );
}

void Facility::CbResultFunctionThread( boost::function< void(bool) > callbackNotifyResult )
{
    callbackNotifyResult( Invoke() );
}