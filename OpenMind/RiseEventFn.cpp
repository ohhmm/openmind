#include "StdAfx.h"
#include "RiseEventFn.h"

RiseEventFn::RiseEventFn( HANDLE e ) : e_(e)
{

}

void RiseEventFn::operator()()
{
	PulseEvent(e_);
}
