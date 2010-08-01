#include "StdAfx.h"
#include <boost/lambda/lambda.hpp>
#include <boost/static_assert.hpp>
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

void Goal::SubscribeOnReach( boost::function <void()> f )
{
    _onReach.push_back(f);
}

void Goal::OnReach()
{
    BOOST_FOREACH(boost::function <void()> f, _onReach)
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
	_reaching = boost::thread(
		boost::bind(&Goal::CompleteReaching, this));
}

bool Goal::CompleteReaching()
{
	Mind mind;
	mind.AddGoal(Goal::ptr_t(this));
	while(!mind.ReachGoals() && IsReachable())
	{
		Sleep(1000);
	}

	return true;
}

bool Goal::IsReachable()
{
	return true;
}
