#pragma once
#include <list>
#include <boost/function.hpp>

class Goal;
class Facility
{
    void CbResultFunctionThread(boost::function< void(bool) > callbackNotifyResult);

public:

    typedef PTRT(Facility) ptr_t;
	typedef PTRT(Goal)     goal_ptr_t;
	Facility(void);
	virtual bool Use(goal_ptr_t goal);
	virtual std::list<goal_ptr_t> ToUse();
    virtual bool Invoke();
	virtual ~Facility(void);

    void AsyncInvoke();
    typedef boost::function< void(bool) >  result_notification_f;
    void AsyncInvoke(result_notification_f callbackNotifyResult);

    virtual bool TryShutdown();
    virtual void ForceShutdown();

};

#include "Goal.h"
