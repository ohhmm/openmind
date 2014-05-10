#pragma once
#include "GoalGenerator.h"

#include <boost/lexical_cast.hpp>

template <class ChildT>
class GeneralGoalGenerator :
	public GoalGenerator
{
	typedef GoalGenerator base_t;
public:
	typedef std::shared_ptr<ChildT>	ptr_t;

	GeneralGoalGenerator(const string_t& name)
		: base_t(name.c_str())
	{
	}

	~GeneralGoalGenerator(void)
	{
	}

	static base_t::ptr_t Make()
	{
		return base_t::ptr_t(
			new ChildT(
				boost::lexical_cast<string_t>(typeid(ChildT).name())
			));
	}
};
