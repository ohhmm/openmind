#include "stdafx.h"

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
    for(auto& f : _onReach)
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
	assert(this);
	auto sharedFromThis = shared_from_this();
	auto completeReaching = [](decltype(sharedFromThis) this_) {
		this_->CompleteReaching();
	};
	_reaching = std::thread(completeReaching, sharedFromThis);
}

bool Goal::CompleteReaching()
{
	assert(this);
	Mind mind;
	mind.AddGoal(shared_from_this());
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
