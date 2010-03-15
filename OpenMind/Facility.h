#pragma once
#include <list>
#include <boost/function.hpp>
#include "Goal.h"

class Goal;
class Facility
{
    void CbResultFunctionThread(boost::function< void(bool) > callbackNotifyResult);

public:

    typedef boost::shared_ptr<Facility> ptr_t;
	typedef boost::shared_ptr<Goal>     goal_ptr_t;
	Facility(void);
	virtual bool Use(goal_ptr_t goal);
	virtual std::list<goal_ptr_t> ToUse();
    virtual bool Invoke();
	virtual ~Facility(void);

    void AsyncInvoke();
    void AsyncInvoke(boost::function< void(bool) > callbackNotifyResult);
};
