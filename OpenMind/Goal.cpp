#include "stdafx.h"
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>
#include "Goal.h"
#include "Mind.h"

Goal::Goal(void)
: res_(0)
{
}

Goal::Goal( parent_generator_ptr_t parentGenerator ) : _parentGenerator(parentGenerator)
{

}

Goal::~Goal(void)
{
	_reaching.join();
}

bool Goal::Reach()
{
	OnReach();
	return true;
}

Goal::container_t Goal::ToReach()
{
	//assert(!"implemented");
	return container_t();
}

void Goal::SubscribeOnReach( std::function <void()> f )
{
    _onReach.push_back(f);
}

void Goal::OnReach()
{
    BOOST_FOREACH(std::function <void()> f, _onReach)
    {
        f();
    }
}

Goal::string_t Goal::SerializedResult()
{
	return _result.str();
}

void Goal::StartReachingAsync()
{
	_reaching = std::thread(
		boost::bind(&Goal::CompleteReaching, this));
}

bool Goal::CompleteReaching()
{
	Mind mind;
	mind.AddGoal(Goal::ptr_t(this));
	while(!mind.ReachGoals() && IsReachable())
	{
		std::chrono::milliseconds duration(1000);
		std::this_thread::sleep_for(duration);
	}

	return true;
}

bool Goal::IsReachable()
{
	return true;
}
