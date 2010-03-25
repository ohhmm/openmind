#include "StdAfx.h"
#include <boost/lambda/lambda.hpp>
#include "Goal.h"

Goal::Goal(void)
: res_(0)
{
}

Goal::Goal( parent_generator_ptr_t parentGenerator ) : _parentGenerator(parentGenerator)
{

}

Goal::~Goal(void)
{
}

bool Goal::Reach()
{
	assert(!"implemented");
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