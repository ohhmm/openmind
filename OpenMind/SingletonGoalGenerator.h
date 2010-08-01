#pragma once
#include "goalgeneratorbase.h"

template <class SelfT>
class SingletonGoalGenerator :
    public GoalGeneratorBase<SelfT>
{
    typedef GoalGeneratorBase<SelfT>    base_t;
    volatile bool generated_;

protected:
    typedef boost::detail::lightweight_mutex  mutex_t;
    volatile mutex_t goalPresenseControl_;

public:

    SingletonGoalGenerator(string_t::pointer name)
        : base_t(name), generated_(false)
    { }

    bool IsNeedToGenerate()
    { return !generated_; }

    void Generated()
    { generated_ = true; }

    void Reached()
    {
        generated_ = false;
		if (IsNeedToGenerate())
		{
			BOOST_FOREACH(need_generate_notification_fn_t& fn, needGenerationNotificators_)
			{
				fn();
			}
		}
    }

};
