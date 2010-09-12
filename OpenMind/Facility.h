#pragma once
#include <list>
#include <boost/function.hpp>

class Goal;
class Facility
{
    void CbResultFunctionThread(boost::function< void(bool) > callbackNotifyResult);

protected:
    Facility(void);

public:

    typedef PTRT(Facility) ptr_t;
	typedef PTRT(Goal)     goal_ptr_t;

	virtual bool Use(goal_ptr_t goal);
	virtual std::list<goal_ptr_t> ToUse();
    virtual bool Invoke();
    virtual bool TryShutdown();
    virtual void ForceShutdown();

	virtual ~Facility(void);

    void AsyncInvoke();
    typedef boost::function< void(bool) >  result_notification_f;
    void AsyncInvoke(result_notification_f callbackNotifyResult);


};

template <class FacilityClass>
class GenericFacility
    : public Facility
{
public:
    static Facility::ptr_t make()
    {
        return Facility::ptr_t(new FacilityClass());
    }
};

#include "Goal.h"
