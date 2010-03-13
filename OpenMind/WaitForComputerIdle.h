#pragma once
#include <boost/function.hpp>
#include "Goal.h"

class WaitForComputerIdle :
	public Goal
{
	boost::function<void()> command_;
public:
	WaitForComputerIdle(boost::function<void()> cmd);
	bool Reach();
	~WaitForComputerIdle(void);
};
